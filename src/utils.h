#include <stdio.h>
#include <stdarg.h>

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

#endif