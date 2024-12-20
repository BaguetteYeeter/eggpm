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

    if (opts.upgrade == 1) {
        struct repo_package *db_pkgs;
        int count = get_all_packages(db, &db_pkgs);
        for (int i = 0; i < count; i++) {
            struct repo_package dbpkg = db_pkgs[i];
            struct repo_package newpkg;
            int res = get_pkg(dbpkg.name, config, &newpkg);
            if (res == 0) {
                res = check_upgrade(db, &newpkg);
                if (res > 0) {
                    newpkg.operation = "upgrade";
                    newpkg.oldversion = dbpkg.version;
                    add_pkg(&packages, &packc, newpkg);
                }
            }
        }
    }

    for (int i = 0; i < opts.packc; i++) {
        int res = 1;
        struct repo_package pkg;

        if (access(opts.packages[i], F_OK) == 0) {
            res = get_info_xml(opts.packages[i], &pkg);
        }

        if (res == 1) {
            res = get_pkg(opts.packages[i], config, &pkg);
        }
        
        if (res == 0) {
            if (opts.install == 1) {
                pkg.operation = "install";
            }
            if (opts.force != 1) {
                res = check_upgrade(db, &pkg);
                if (res == 0) {
                    printf("Package `%s` already installed\n", pkg.name);
                    continue;
                }
            }
            add_pkg(&packages, &packc, pkg);
        } else {
            printf("ERROR: Package `%s` not found\n", opts.packages[i]);
            exit(1);
        }
    }

    while (1 == 1) {
        int changes = 0;
        for (int i = 0; i < packc; i++) {
            int count = 0;
            char** parts = split_string(packages[i].rundepends, " ", &count);
            for (int j = 0; j < count; j++) {
                int found = 0;
                for (int k = 0; k < packc; k++) {
                    if (strcmp(packages[k].name, parts[j]) == 0) {
                        found = 1;
                    }
                }
                if (found == 0) {
                    struct repo_package pkg;
                    int res = get_pkg(parts[j], config, &pkg);
                    if (res == 0) {
                        pkg.operation = "install";
                        res = check_upgrade(db, &pkg);
                        if (res == 0) {
                            continue;
                        }
                        add_pkg(&packages, &packc, pkg);
                        changes++;
                    } else {
                        printf("ERROR: Package `%s` not found\n", parts[j]);
                    }
                }
            }
        }
        if (changes == 0) {
            break;
        }
    }

    if (packc > 0) {
        printf("\n");

        for (int i = 0; i < packc; i++) {
            struct repo_package pkg = packages[i];
            if (strcmp(pkg.operation, "upgrade") == 0) {
                printf("%s (%s -> %s) will be upgraded\n", pkg.name, pkg.oldversion, pkg.version);
            } else {
                printf("%s (%s) will be %sed\n", pkg.name, pkg.version, pkg.operation);
            }
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
            install_package(pkg, opts);
            printf("done\n");
        }

        printf("\n---Processing packages---\n");
        for (int i = 0; i < packc; i++) {
            pkg = packages[i];
            char* install_sh;
            int extra = get_install_sh(get_pkg_filename(pkg), &install_sh);
            if (extra == 0) {
                printf("%s-%s... ", pkg.name, pkg.version);
                fflush(stdout);
                system(install_sh);
                printf("done\n");
            }
        }

        long installtime = time(NULL);

        for (int i = 0; i < packc; i++) {
            pkg = packages[i];
            pkg.installdate = installtime;
            if (get_package(db, pkg.name, NULL) != 0) {
                add_package(db, pkg.name, pkg.version, pkg.architecture, pkg.repository, pkg.description, pkg.installdate, pkg.size);
            } else {
                update_package(db, pkg);
            }
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
