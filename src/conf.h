#ifndef CONF_H
#define CONF_H

struct conf {
    char** repositories;
    int repoc;
    char* arch;
    char* repo_prefix;
    char* repo_path;
};

char* get_arch();
struct conf readconf();

#endif