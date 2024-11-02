#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <regex.h>

#include "utils.h"

struct build_pkg {
    char* name;
    char* version;
    char* makedepends;
    char* rundepends;
    char* url;
    char* checksum;
    char** stages;
};

void build_package(char* name) {
    char* path = catstring(name, "/build.sh", NULL);

    if (access(path, F_OK) != 0) {
        printf("Can't find build file");
        exit(1);
    }

    char** stages = (char**) malloc(sizeof(char*) * 100);
    for (int i = 0; i < 100; i++) {
        stages[i] = NULL;
    }
    struct build_pkg pkg = {"", "", "", "", "", "", stages};

    FILE *fp = popen(catstring("source ", path, " && set", NULL), "r");

    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        line[strcspn(line, "\n")] = 0;
        //char** parts = split_string(line, "=", (int*) -1);
        char** parts = split_string_no(line, "=", 1);
        if (strcmp(parts[0], "name") == 0) {
            pkg.name = parts[1];
        } else if (strcmp(parts[0], "version") == 0) {
            pkg.version = parts[1];
        } else if (strcmp(parts[0], "url") == 0) {
            pkg.url = parts[1];
        } else if (strcmp(parts[0], "checksum") == 0) {
            pkg.checksum = parts[1];
        } else if (strcmp(parts[0], "makedepends") == 0) {
            pkg.makedepends = parts[1];
        } else if (strcmp(parts[0], "rundepends") == 0) {
            pkg.rundepends = parts[1];
        } else if (strstart(parts[0], "stage") == 0) {
            char *pattern = "stage([0-9][0-9])";
            regex_t regex;
            int reti = regcomp(&regex, pattern, REG_EXTENDED);
            regmatch_t matches[2];
            reti = regexec(&regex, parts[0], 2, matches, 0);
            int length = matches[1].rm_eo - matches[1].rm_so;
            char stageno[length + 1];
            snprintf(stageno, length + 1, "%.*s", length, parts[0] + matches[1].rm_so);
            int sno = strtol((char*) stageno, NULL, 10);
            if (parts[1][0] == '\'') {
                parts[1]++;
                parts[1][strlen(parts[1])-1] = 0;
            }
            if (stages[sno] == NULL) {
                stages[sno] = parts[1];
            } else {
                stages[sno] = catstring(stages[sno], "\n", parts[1], NULL);
            }
        }
    }

    printf("Package name: %s\n", pkg.name);
    printf("Package version: %s\n", pkg.version);
    printf("Package url: %s\n", pkg.url);
    printf("Package checksum: %s\n", pkg.checksum);
    printf("Stage 10: %s\n", pkg.stages[10]);
    printf("Stage 50: %s\n", pkg.stages[50]);
}