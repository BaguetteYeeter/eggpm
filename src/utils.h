#include <stdio.h>
#include <stdarg.h>

#ifndef UTILS_H
#define UTILS_H

char* catstring(char* string, ...);
long fsize(FILE *fp);
void add_string_list(char*** list, int* listc, char* string);
char** split_string(char *string, char *split, int *count);

#endif