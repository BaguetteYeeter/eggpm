#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

#include "build.h"
#include "conf.h"
#include "config.h"
#include "database.h"
#include "install.h"
#include "parser.h"
#include "repo.h"
#include "utils.h"

int main(int argc, char* argv[]) {
    struct options opts = parse(argc, argv);

    struct conf config = readconf();

    if (config.repoc < 1) {
        printf("Warning: No repositories found\n");
    }
 
    if (opts.update_repo == 1) {
        download_repo(config);
    }

    if (opts.build_package == 1) {
        for (int i = 0; i < opts.packc; i++) {
            build_package(opts.packages[i], config, opts);
        }
        exit(0);
    }

    sqlite3 *db;
    char* db_location = catstring(VAR_PREFIX, "/eggpm/pkgdb.db", NULL);

    db = create_database(db_location);

    struct repo_package *packages = (struct repo_package*) malloc(sizeof(struct repo_package) * opts.packc);
    int packc = 0;

    for (int i = 0; i < opts.packc; i++) {
        int res = 1;
        struct repo_package pkg;

        if (access(opts.packages[i], F_OK) == 0) {
            res = get_info_xml(opts.packages[i], &pkg);
        }
        if (res == 1) {
            for (int j = 0; j < config.repoc; j++) {
                res = search_repo(config, j, opts.packages[i], &pkg);
                if (res == 0) {
                    break;
                }
            }
        }
        
        if (res == 0) {
            if (opts.force != 1) {
                if (get_package(db, pkg.name, NULL) == 0) {
                    printf("Package `%s` already installed\n", pkg.name);
                    continue;
                }
            }
            if (opts.install == 1) {
                pkg.operation = "install";
            }
            packages[packc] = pkg;
            packc++;
        } else {
            printf("ERROR: Package `%s` not found\n", opts.packages[i]);
            exit(1);
        }
    }

    if (packc > 0) {
        printf("\n");

        for (int i = 0; i < packc; i++) {
            struct repo_package pkg = packages[i];
            printf("%s (%s) will be %sed\n", pkg.name, pkg.version, pkg.operation);
        }

        if (opts.yes != 1) {
            printf("\nDo you want to continue [y/n]? ");
            fflush(stdout);
            char* yesno = (char*) malloc(sizeof(char) * 30);
            scanf("%29s", yesno);
            if (strcmp("y", yesno) != 0) {
                printf("Aborting!\n");
                exit(0);
            }
        }

        struct repo_package pkg;

        printf("\n---Downloading packages---\n");
        for (int i = 0; i < packc; i++) {
            pkg = packages[i];
            if (pkg.local) {
                continue;
            }
            printf("%s... ", pkg.url);
            fflush(stdout);
            download_package(pkg);
            printf("done\n");
        }
        
        printf("\n---Installing packages---\n");
        for (int i = 0; i < packc; i++) {
            pkg = packages[i];
            printf("%s-%s... ", pkg.name, pkg.version);
            fflush(stdout);
            install_package(pkg);
            printf("done\n");
        }

        long installtime = time(NULL);
        char* installdate = (char*) malloc(sizeof(char*) * 16);
        snprintf(installdate, 15, "%ld", installtime);

        for (int i = 0; i < packc; i++) {
            pkg = packages[i];
            add_package(db, pkg.name, pkg.version, pkg.architecture, pkg.repository, pkg.description, installdate, pkg.size);
        }

        if (packc == 1) {
            printf("\nSuccessfully installed %d package\n", packc);
        } else {
            printf("\nSuccessfully installed %d packages\n", packc);
        }
    }

    sqlite3_close(db);

    return 0;
}
