#include "conf.h"
#include "parser.h"

#ifndef BUILD_H
#define BUILD_H

struct build_pkg {
    char* name;
    char* version;
    char* arch;
    char* description;
    char* makedepends;
    char* rundepends;
    char* url;
    char* checksum;
    char** stages;
};

void build_package(char* name, struct conf config, struct options opts);

#endif