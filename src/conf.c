#include <config.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "utils.h"

struct conf {
    char** repositories;
    int repoc;
};

struct conf readconf() {
    struct conf result;

    result.repoc = 0;
    result.repositories = (char**) malloc(sizeof(char*));

    char* filename = catstring(ETC_PREFIX, "/eggpm.conf");

    if (access(filename, F_OK) != 0) {
        return result;
    }

    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "Failed to open config file");
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
        if (!strncmp(lines[i], "repository=", 11)) {
            char* repo = split_string(lines[i], "=", (int*) -1)[1];
            add_string_list(result.repositories, &result.repoc, repo);
        }
    }
    
    return result;
}