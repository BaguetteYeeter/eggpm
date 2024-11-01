#include <curl/curl.h>
#include <stdio.h>
#include <config.h>
#include <regex.h>
#include <string.h>
#include <stdlib.h>

#include "conf.h"
#include "utils.h"

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

void download_repo(struct conf config) {
    char *pattern = "https?://([A-Za-z0-9\\.]+).*/([A-Za-z0-9_\\.]+)/?";

    for (int i = 0; i < config.repoc; i++) {
        char* url = config.repositories[i];

        CURL *curl = curl_easy_init();

        if (!curl) {
            printf("Error when starting curl");
            exit(1);
        }

        regex_t regex;
        int reti = regcomp(&regex, pattern, REG_EXTENDED);
        if (reti) {
            printf("regex issue %d\n", reti);
            exit(1);
        }
        regmatch_t matches[3];
        reti = regexec(&regex, url, 3, matches, 0);

        if (reti) {
            printf("URL issue %d %s\n", reti, url);
            exit(1);
        }

        int length = matches[1].rm_eo - matches[1].rm_so;
        char domain[length + 1];
        snprintf(domain, length + 1, "%.*s", length, url + matches[1].rm_so);

        length = matches[2].rm_eo - matches[2].rm_so;
        char path[length + 1];
        snprintf(path, length + 1, "%.*s", length, url + matches[2].rm_so);

        regfree(&regex);

        char* directory = catstring(VAR_PREFIX, "/eggpm/repos/", domain, NULL);
        system(catstring("mkdir -p ", directory, NULL));

        FILE* fp = fopen(catstring(directory, "/", path, NULL), "wb");

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            printf("Curl error");
            exit(1);
        }

        long code;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

        if (code != 200) {
            printf("WARNING: Repo returned code %ld\n", code);
        }

        fclose(fp);
        curl_easy_cleanup(curl);

        printf("Downloaded repo %s\n", url);
    }
}