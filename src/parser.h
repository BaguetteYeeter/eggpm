#ifndef PARSER_H
#define PARSER_H

struct options {
    char** packages;
    int packc;
    int update_repo;
    int build_package;
    int install;
    int force;
    int keep;
    int yes;
};

struct options parse(int argc, char *argv[]);

#endif