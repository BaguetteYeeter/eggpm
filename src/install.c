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

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

char* calculate_sha256(FILE *fp) {
    unsigned char* result = (unsigned char*) malloc(sizeof(unsigned char) * (EVP_MAX_MD_SIZE));
    long prev=ftell(fp);
    fseek(fp, 0, SEEK_SET);

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();

    const EVP_MD *md = EVP_sha256();
    EVP_DigestInit_ex(mdctx, md, NULL);

    unsigned char buffer[1024];
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, 1024, fp)) != 0) {
        EVP_DigestUpdate(mdctx, buffer, bytes_read);
    }

    unsigned int sha256_length;
    EVP_DigestFinal_ex(mdctx, result, &sha256_length);
    EVP_MD_CTX_free(mdctx);

    char *hex = malloc((sha256_length * 2 + 1) * sizeof(char));

    // Convert the byte array to a hex string
    for (unsigned int i = 0; i < sha256_length; i++) {
        snprintf(&hex[i * 2], sha256_length*2+1, "%02x", result[i]);
    }
    hex[sha256_length * 2] = '\0';

    fseek(fp,prev,SEEK_SET);
    return hex;
}

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
    CURL *curl = curl_easy_init();

    if (!curl) {
        printf("Error when starting curl");
        exit(1);
    }

    char* filename = get_filename(pkg);

    FILE* fp = fopen(filename, "wb");

    curl_easy_setopt(curl, CURLOPT_URL, pkg.url);
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
        printf("WARNING: Package returned code %ld\n", code);
    }

    fclose(fp);
    fp = fopen(filename, "rb");

    char* checksum = calculate_sha256(fp);

    if (strcmp(checksum, pkg.checksum)) {
        printf("Checksums don't match %s %s\n", checksum, pkg.checksum);
        exit(1);
    }

    fclose(fp);
    curl_easy_cleanup(curl);
}
