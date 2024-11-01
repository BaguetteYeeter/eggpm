#ifndef INSTALL_H
#define INSTALL_H

#include <stdio.h>
#include "repo.h"

char* calculate_sha256(FILE *fp);
void download_package(struct repo_package pkg);

#endif