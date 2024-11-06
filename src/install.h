#ifndef INSTALL_H
#define INSTALL_H

#include <stdio.h>
#include "repo.h"

char* get_pkg_filename(struct repo_package pkg);
int get_info_xml(char* filename, struct repo_package* out_pkg);
void download_package(struct repo_package pkg);
void install_package(struct repo_package pkg);

#endif