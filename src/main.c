#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <config.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "utils.h"
#include "database.h"

struct options {
    char** packages;
    int packc;
};

struct options parse(int argc, char *argv[]) {
    int opt;
    struct options opts;
    int arg_index = 0;

    char** packages = (char**) malloc(sizeof(char*) * argc);

    for (int i = 0; i < argc; i++) {
        packages[i] = (char*) malloc(sizeof(char) * (strlen(argv[i]) + 1));
    }

    struct option long_options[] = {
        {"version", no_argument, 0, 'V'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "hV", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                printf("help menu coming soon\n");
                exit(0);
            case 'V':
                printf("%s\n", PACKAGE_STRING);
                exit(0);
            default:
                exit(1);
        }
    }

    while (optind < argc) {
        packages[arg_index++] = argv[optind++];
    }

    opts.packages = packages;
    opts.packc = optind - 1;

    return opts;
}

int main(int argc, char* argv[]) {
    struct options opts = parse(argc, argv);

    for (int i = 0; i < argc; i++) {
        printf("%s\n", opts.packages[i]);
    }
    printf("%d\n", opts.packc);

    printf("EggPM installed in %s\n", INSTALL_PREFIX);
    printf("/var is at %s\n", VAR_PREFIX);

    sqlite3 *db;
    char* db_location = catstring(VAR_PREFIX, "/eggpm/pkgdb.db");

    db = create_database(db_location);
    printf("Opened pkgdb\n");

    add_package(db, "hello", "2.12.1", "x86_64-darwin", "", "", "1970-01-01T00:00:00+00:00", 187007);
    
    list_all_packages(db);

    sqlite3_close(db);

    return 0;
}