#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <config.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utils.h"
#include "database.h"
#include "conf.h"
#include "repo.h"
#include "install.h"
#include "build.h"
#include "parser.h"

int main(int argc, char* argv[]) {
    struct options opts = parse(argc, argv);

    struct conf config = readconf();

    if (config.repoc < 1) {
        printf("Warning: No repositories found");
    }
 
    if (opts.update_repo == 1) {
        download_repo(config);
    }

    if (opts.build_package == 1) {
        for (int i = 0; i < opts.packc; i++) {
            build_package(opts.packages[i]);
        }
        exit(0);
    }

    sqlite3 *db;
    char* db_location = catstring(VAR_PREFIX, "/eggpm/pkgdb.db", NULL);

    db = create_database(db_location);

    if (opts.install == 1) {
        int res;
        for (int i = 0; i < opts.packc; i++) {
            struct repo_package pkg;
            for (int j = 0; j < config.repoc; j++) {
                res = search_repo(config, j, opts.packages[i], &pkg);
                if (res == 0) {
                    break;
                }
            }
            if (res == 0) {
                printf("Name: %s | Version: %s\n", pkg.name, pkg.version);
                download_package(pkg);
                install_package(pkg);
                printf("%s successfully installed\n", pkg.name);
            } else {
                printf("ERROR: Package `%s` not found\n", opts.packages[i]);
                exit(1);
            }
        }
    }

    sqlite3_close(db);

    return 0;
}
