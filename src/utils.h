#include <stdarg.h>
#include <stdio.h>
#include <sqlite3.h>

#include "conf.h"
#include "repo.h"

#ifndef UTILS_H
#define UTILS_H

char* catstring(char* string, ...);
long fsize(FILE *fp);
void add_string_list(char*** list, int* listc, char* string);
char** split_string(char *string, char *split, int *count);
char** split_string_no(char *string, char *split, int max);
int strstart(char* string, char* start);

void download_file(char* url, char* filename, char* checksum);
char* get_filename_url(char* url);

char* calculate_sha256(FILE *fp);
char* get_sha256(char* filename);

int get_pkg(char* name, struct conf config, struct repo_package *out_pkg);
void add_pkg(struct repo_package** packages, int *packc, struct repo_package pkg);
int check_upgrade(sqlite3 *db, struct repo_package *pkg);

int check_exists(struct repo_package *packages, int packc, char* name, char* version);

#endif