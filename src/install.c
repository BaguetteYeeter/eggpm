#include <archive.h>
#include <archive_entry.h>
#include <stdio.h>
#include <curl/curl.h>
#include <regex.h>
#include <string.h>
#include <config.h>
#include <openssl/evp.h>
#include <stdlib.h>

#include "repo.h"
#include "utils.h"

char* get_filename(struct repo_package pkg) {
    char *pattern = "https?://[A-Za-z0-9\\.\\-]+.*/([A-Za-z0-9_\\.\\-]+)/?";

    regex_t regex;
    int reti = regcomp(&regex, pattern, REG_EXTENDED);

    regmatch_t matches[2];
    reti = regexec(&regex, pkg.url, 3, matches, 0);

    if (reti) {
        printf("URL issue %d %s\n", reti, pkg.url);
        exit(1);
    }

    int length = matches[1].rm_eo - matches[1].rm_so;
    char* path = (char*) malloc(sizeof(char)*(length+1));
    snprintf(path, length + 1, "%.*s", length, pkg.url + matches[1].rm_so);

    regfree(&regex);

    char* directory = catstring(VAR_PREFIX, "/cache/eggpm", NULL);
    system(catstring("mkdir -p ", directory, NULL));

    return catstring(directory, "/", path, NULL);
}

void download_package(struct repo_package pkg) {
    download_file(pkg.url, get_filename(pkg), pkg.checksum);
}
