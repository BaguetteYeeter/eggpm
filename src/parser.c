#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <config.h>
#include <string.h>

struct options {
    char** packages;
    int packc;
    int update_repo;
    int build_package;
    int install;
};

void print_help(char* program_name) {
    printf(
        "EggPM Package Manager\n"
        "Usage: %s [OPTION] [PACKAGES...]\n\n"
        "  -b, --build          build a package\n"
        "  -i, --install        install a package\n"
        "  -S, --update-repo    update the repository\n\n"
        "  --help       display this help and exit\n"
        "  --version    output version information and exit\n\n"

        "Report bugs to: baguetteyeeter@icloud.com\n"
        "EggPM home page: <https://github.com/BaguetteYeeter/eggpm>\n"
        , program_name
    );
}

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
                print_help(argv[0]);
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
                print_help(argv[0]);
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