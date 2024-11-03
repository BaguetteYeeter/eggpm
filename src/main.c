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

struct options {
    char** packages;
    int packc;
    int update_repo;
    int build_package;
    int install;
};

struct options parse(int argc, char *argv[]) {
    int opt;
    struct options opts;
    int arg_index = 0;

    opts.update_repo = 0;
    opts.build_package = 0;
    opts.install = 0;

    char** packages = (char**) malloc(sizeof(char*) * argc);
    for (int i = 0; i < argc; i++) {
        packages[i] = (char*) malloc(sizeof(char) * (strlen(argv[i]) + 1));
    }

    struct option long_options[] = {
        {"version", no_argument, 0, 'V'},
        {"help", no_argument, 0, 'h'},
        {"update-repo", no_argument, 0, 'S'},
        {"build", no_argument, 0, 'b'},
        {"install", no_argument, 0, 'i'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "hVSbi", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                printf("help menu coming soon\n");
                exit(0);
            case 'V':
                printf("%s\n", PACKAGE_STRING);
                exit(0);
            case 'S':
                opts.update_repo = 1;
                continue;
            case 'b':
                opts.build_package = 1;
                continue;
            case 'i':
                opts.install = 1;
                continue;
            default:
                exit(1);
        }
    }

	opts.packc = 0;
    while (optind < argc) {
        packages[arg_index++] = argv[optind++];
        opts.packc++;
    }

    opts.packages = packages;

    return opts;
}

int main(int argc, char* argv[]) {
    struct options opts = parse(argc, argv);

    printf("EggPM installed in %s\n", INSTALL_PREFIX);
    printf("/var is at %s\n", VAR_PREFIX);
    printf("/etc is at %s\n", ETC_PREFIX);

    struct conf config = readconf();

    for (int i = 0; i < config.repoc; i++) {
        printf("Repository at %s\n", config.repositories[i]);
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
    printf("Opened pkgdb\n");

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
    
    list_all_packages(db);

    sqlite3_close(db);

    return 0;
}
