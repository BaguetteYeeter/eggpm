#include <stdio.h>

#ifndef UTILS_H
#define UTILS_H

char* catstring(char* string1, char* string2);
long fsize(FILE *fp);
void add_string_list(char** list, int* listc, char* string);
char** split_string(char *string, char *split, int *count);

#endif