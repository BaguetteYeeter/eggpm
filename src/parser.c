#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "parser.h"

void print_help(char* program_name) {
    printf(
        "EggPM Package Manager\n"
        "Usage: %s [OPTION] [PACKAGES...]\n\n"
        "  -b, --build          build a package\n"
        "  -f, --force          do not check installed packages\n"
        "  -i, --install        install a package\n"
        "  -k, --keep           do not delete extra files\n"
        "      --skip-stages    skip downloading and building in --build\n"
        "  -R, --root           set install root (default /tmp/eggpmtest)\n"
        "  -S, --update-repo    update the repository\n"
        "  -u, --upgrade        upgrade all packages\n"
        "  -y, --yes            automatically answer questions\n"
        "\n  --help       display this help and exit\n"
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
    opts.force = 0;
    opts.keep = 0;
    opts.yes = 0;
    opts.upgrade = 0;
    opts.skip_stages = 0;
    opts.root = "/tmp/eggpmtest";

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
        {"force", no_argument, 0, 'f'},
        {"keep", no_argument, 0, 'k'},
        {"yes", no_argument, 0, 'y'},
        {"upgrade", no_argument, 0, 'u'},
        {"skip-stages", no_argument, 0, 1},
        {"root", required_argument, 0, 'R'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    while ((opt = getopt_long(argc, argv, "hVSbifkyuR:", long_options, &option_index)) != -1) {
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
            case 'f':
                opts.force = 1;
                continue;
            case 'k':
                opts.keep = 1;
                continue;
            case 'y':
                opts.yes = 1;
                continue;
            case 'u':
                opts.upgrade = 1;
                continue;
            case 1:
                opts.skip_stages = 1;
                continue;
            case 'R':
                opts.root = strdup(optarg);
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