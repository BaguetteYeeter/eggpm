#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <config.h>
#include <string.h>

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

    return 0;
}