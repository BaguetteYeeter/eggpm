#include <config.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"
#include "conf.h"

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

struct conf readconf() {
    struct conf result;

    result.repoc = 0;
    result.repositories = (char**) malloc(sizeof(char*));
    result.repo_prefix = "YOUR_URL";
    result.repo_path = NULL;

    char* filename = catstring(ETC_PREFIX, "/eggpm.conf", NULL);

    if (access(filename, F_OK) != 0) {
        return result;
    }

    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open config file\n");
        exit(1);
    }

    int filesize = fsize(file);

    char* buffer = (char*) malloc(filesize + 1);
    if (buffer == NULL) {
        exit(1);
    }

    fread(buffer, 1, filesize, file);
    buffer[filesize] = '\0';

    int linec;
    char** lines = split_string(buffer, "\n", &linec);

    for (int i = 0; i < linec; i++) {
        if (!strstart(lines[i], "repository=")) {
            char* repo = split_string(lines[i], "=", (int*) -1)[1];
            add_string_list(&result.repositories, &result.repoc, repo);
        } else if (!strstart(lines[i], "repo_prefix=")) {
            char* repo = split_string(lines[i], "=", (int*) -1)[1];
            result.repo_prefix = repo;
        } else if (!strstart(lines[i], "repo_path=")) {
            char* repo = split_string(lines[i], "=", (int*) -1)[1];
            result.repo_path = repo;
        }
    }

    result.arch = get_arch();
    
    return result;
}
