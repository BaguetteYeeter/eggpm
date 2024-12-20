#include <stdio.h>

#include "repo.h"

#ifndef INSTALL_H
#define INSTALL_H

char* get_pkg_filename(struct repo_package pkg);
int get_info_xml(char* filename, struct repo_package* out_pkg);
int get_install_sh(char *filename, char **output);
void download_package(struct repo_package pkg);
void install_package(struct repo_package pkg, struct options opts);

#endif