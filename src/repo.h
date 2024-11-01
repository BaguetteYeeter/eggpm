#ifndef REPO_H
#define REPO_H

#include "utils.h"

struct repo_package {
    char* name;
    char* version;
    char* architecture;
    char* repository;
    char* description;
    long int size;
    char* url;
    char* checksum;
};

char* get_repo_name(struct conf config, int index);

void download_repo(struct conf config);
int search_repo(struct conf config, int repo_index, char* pkgname, struct repo_package* outpkg);

#endif