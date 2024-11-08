#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <curl/curl.h>
#include <openssl/evp.h>
#include <regex.h>

char* catstring(char* string, ...) {
    int length = 1;
    length = strlen(string);

    va_list ap;
    va_start(ap, string);

    char* arg;
    while ((arg = va_arg(ap, char*)) != NULL) {
        length += strlen(arg);
    }
    va_end(ap);

    char* result = (char *) malloc((length+1) * sizeof(char));
    if (result == NULL) {
        exit(1);
    }

    strcpy(result, string);

    va_start(ap, string);
    while ((arg = va_arg(ap, char*)) != NULL) {
        strcat(result, arg);
    }
    va_end(ap);

    return result;
}

// https://stackoverflow.com/a/238609
long fsize(FILE *fp) {
    long prev=ftell(fp);
    fseek(fp, 0L, SEEK_END);
    long sz=ftell(fp);
    fseek(fp,prev,SEEK_SET);
    return sz;
}

void add_string_list(char*** list, int* listc, char* string) {
    int count = *listc;
    char** result = realloc(*list, sizeof(char*) * (count + 1));
    if (result == NULL) {
        exit(1);
    }

    result[count] = strdup(string);
    if (result[count] == NULL) {
        exit(1);
    }

    count++;
    *listc = count;
    *list = result;
}

char** split_string(char *string, char *split, int *count) {
    char** result = (char**) malloc(sizeof(char*));
    int size = 0;

    char *token = strtok(string, split);
    while (token) {
        add_string_list(&result, &size, token);
        token = strtok(NULL, split);
    }

    if (count != (int*) -1) {
        *count = size;
    }
    return result;
}

char** split_string_no(char *string, char *split, int max) {
    char** result = (char**) malloc(sizeof(char*) * (max+1));
    int size = 0;

    if (strstr(string, split) == NULL) {
        for (int i = 0; i < max+1; i++) {
            result[i] = NULL;
        }
        add_string_list(&result, &size, string);
        return result;
    }

    char *token = strtok(string, split);
    while (token) {
        add_string_list(&result, &size, token);
        if (size >= max) {
            add_string_list(&result, &size, strtok(NULL, ""));
            break;
        }
        token = strtok(NULL, split);
    }

    return result;
}

int strstart(char* string, char* start) {
    return strncmp(string, start, strlen(start));
}

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

    for (unsigned int i = 0; i < sha256_length; i++) {
        snprintf(&hex[i * 2], sha256_length*2+1, "%02x", result[i]);
    }
    hex[sha256_length * 2] = '\0';

    fseek(fp,prev,SEEK_SET);
    return hex;
}

char* get_sha256(char* filename) {
    if (access(filename, F_OK) != 0) {
        printf("Failed to read `%s`\n", filename);
    }
    FILE* fp = fopen(filename, "rb");
    char* checksum = calculate_sha256(fp);
    fclose(fp);
    return checksum;
}

void download_file(char* url, char* filename, char* checksum) {
    CURL *curl = curl_easy_init();

    if (!curl) {
        printf("Error when starting curl\n");
        exit(1);
    }

    FILE* fp = fopen(filename, "wb");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
        printf("Curl error\n");
        exit(1);
    }

    long code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

    if (code != 200) {
        printf("WARNING: Download returned code %ld\n", code);
    }

    fclose(fp);
    fp = fopen(filename, "rb");

    char* our_checksum = calculate_sha256(fp);

    if (strcmp(our_checksum, checksum)) {
        printf("Checksums don't match\n");
        exit(1);
    }

    fclose(fp);
    curl_easy_cleanup(curl);
}

char* get_filename_url(char* url) {
    char *pattern = "https?://[A-Za-z0-9\\.\\-]+.*/([A-Za-z0-9_\\.\\-]+)/?";

    regex_t regex;
    int reti = regcomp(&regex, pattern, REG_EXTENDED);

    regmatch_t matches[2];
    reti = regexec(&regex, url, 2, matches, 0);

    int length = matches[1].rm_eo - matches[1].rm_so;
    char* path = (char*) malloc(sizeof(char)*(length+1));
    snprintf(path, length + 1, "%.*s", length, url + matches[1].rm_so);

    regfree(&regex);

    return path;
}
