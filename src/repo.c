#include <curl/curl.h>
#include <libxml/parser.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "conf.h"
#include "config.h"
#include "repo.h"
#include "utils.h"

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) {
    size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
    return written;
}

char* get_repo_name(struct conf config, int index) {
    char *pattern = "https?://([A-Za-z0-9\\.]+).*/([A-Za-z0-9_\\.]+)/?";
    char *url = config.repositories[index];
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

    return catstring(directory, "/", path, NULL);
}

void download_repo(struct conf config) {
    for (int i = 0; i < config.repoc; i++) {
        char* url = config.repositories[i];

        printf("Updating repository `%s`... ", url);
        fflush(stdout);

        CURL *curl = curl_easy_init();

        if (!curl) {
            printf("Error when starting curl\n");
            exit(1);
        }

        char* path = get_repo_name(config, i);

        FILE* fp = fopen(path, "wb");

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
            printf("WARNING: Repo returned code %ld\n", code);
        }

        fclose(fp);
        curl_easy_cleanup(curl);

        printf("done\n");
    }
}

int search_repo(struct conf config, int repo_index, char* pkgname, struct repo_package* outpkg) {
    xmlInitParser();

    char* path = get_repo_name(config, repo_index);

    xmlDoc *doc = xmlReadFile(path, NULL, 0);
    if (doc == NULL) {
        printf("Error reading repo\n");
        exit(1);
    }

    struct repo_package pkg = {"", "", "", config.repositories[repo_index], "", 0, "", "", "", 0, 0, "", ""};
    int found = 0;

    //i dont understand any of this
    xmlNode *root_element = xmlDocGetRootElement(doc);
    for (xmlNode *currentNode = root_element->children; currentNode; currentNode = currentNode->next) {
        if (currentNode->type == XML_ELEMENT_NODE && xmlStrcmp(currentNode->name, (const xmlChar *)"package") == 0) {
            xmlChar *name = xmlGetProp(currentNode, (const xmlChar *)"name");
            if (strcmp((char*) name, pkgname)) {
                continue;
            } else {
                found = 1;
            }

            for (xmlNode *child = currentNode->children; child; child = child->next) {
                if (child->type == XML_ELEMENT_NODE) {
                    if (xmlStrcmp(child->name, (const xmlChar *)"name") == 0) {
                        xmlChar *name = xmlNodeGetContent(child);
                        pkg.name = (char*) name;
                    } else if (xmlStrcmp(child->name, (const xmlChar *)"version") == 0) {
                        xmlChar *version = xmlNodeGetContent(child);
                        pkg.version = (char*) version;
                    } else if (xmlStrcmp(child->name, (const xmlChar *)"architecture") == 0) {
                        xmlChar *arch = xmlNodeGetContent(child);
                        pkg.architecture = (char*) arch;
                        if (strcmp((char*) arch, config.arch) != 0) {
                            found = 0;
                            break;
                        }
                    } else if (xmlStrcmp(child->name, (const xmlChar *)"description") == 0) {
                        xmlChar *desc = xmlNodeGetContent(child);
                        pkg.description = (char*) desc;
                    } else if (xmlStrcmp(child->name, (const xmlChar *)"size") == 0) {
                        xmlChar *size = xmlNodeGetContent(child);
                        pkg.size = strtol((char*) size, NULL, 10);
                    } else if (xmlStrcmp(child->name, (const xmlChar *)"url") == 0) {
                        xmlChar *url = xmlNodeGetContent(child);
                        pkg.url = (char*) url;
                    } else if (xmlStrcmp(child->name, (const xmlChar *)"checksum") == 0) {
                        xmlChar *cs = xmlNodeGetContent(child);
                        pkg.checksum = (char*) cs;
                    } else if (xmlStrcmp(child->name, (const xmlChar *)"dependencies") == 0) {
                        xmlChar *dependencies = xmlNodeGetContent(child);
                        pkg.rundepends = (char*) dependencies;
                    }
                }
            }

            if (found == 1) {
                break;
            }
        }
    }

    *outpkg = pkg;
    return 1 - found;
}
