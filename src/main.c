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

char* get_arch() {
    char* arch = "unknown";

    #if defined(__x86_64__) || defined(_M_X64)
        #if defined(__APPLE__) && defined(__MACH__)
            arch = "x86_64-darwin";
        #else
            arch = "x86_64";
        #endif
    #elif defined(__aarch64__)
        #if defined(__APPLE__) && defined(__MACH__)
            arch = "arm64-darwin";
        #else
            arch = "arm64";
        #endif
    #endif

    return arch;
}

int main(int argc, char* argv[]) {
    struct options opts = parse(argc, argv);

    char* arch = get_arch();
    printf("Architecture: %s\n", arch);

    struct conf config = readconf();

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
            printf("Name: %s | Version: %s\n", pkg.name, pkg.version);
            download_package(pkg);
            install_package(pkg);
        }
    }

    sqlite3_close(db);

    return 0;
}
