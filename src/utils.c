#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

char* catstring(char* string, ...) {
    int length = 1;
    length = strlen(string);

    va_list ap;
    va_start(ap, string);

    char* arg;
    while ((arg = va_arg(ap, char*)) != NULL) {
        length += strlen(arg);
    }
    va_end(ap);

    char* result = (char *) malloc((length+1) * sizeof(char));
    if (result == NULL) {
        exit(1);
    }

    strcpy(result, string);

    va_start(ap, string);
    while ((arg = va_arg(ap, char*)) != NULL) {
        strcat(result, arg);
    }
    va_end(ap);

    return result;
}

// https://stackoverflow.com/a/238609
long fsize(FILE *fp) {
    long prev=ftell(fp);
    fseek(fp, 0L, SEEK_END);
    long sz=ftell(fp);
    fseek(fp,prev,SEEK_SET);
    return sz;
}

void add_string_list(char*** list, int* listc, char* string) {
    int count = *listc;
    char** result = realloc(*list, sizeof(char*) * (count + 1));
    if (result == NULL) {
        exit(1);
    }

    result[count] = strdup(string);
    if (result[count] == NULL) {
        exit(1);
    }

    count++;
    *listc = count;
    *list = result;
}

char** split_string(char *string, char *split, int *count) {
    char** result = (char**) malloc(sizeof(char*));
    int size = 0;

    char *token = strtok(string, split);
    while (token) {
        add_string_list(&result, &size, token);
        token = strtok(NULL, split);
    }

    if (count != (int*) -1) {
        *count = size;
    }
    return result;
}

char** split_string_no(char *string, char *split, int max) {
    char** result = (char**) malloc(sizeof(char*) * (max+1));
    int size = 0;

    if (strstr(string, split) == NULL) {
        for (int i = 0; i < max+1; i++) {
            result[i] = NULL;
        }
        add_string_list(&result, &size, string);
        return result;
    }

    char *token = strtok(string, split);
    while (token) {
        add_string_list(&result, &size, token);
        if (size >= max) {
            add_string_list(&result, &size, strtok(NULL, ""));
            break;
        }
        token = strtok(NULL, split);
    }

    return result;
}

int strstart(char* string, char* start) {
    return strncmp(string, start, strlen(start));
}