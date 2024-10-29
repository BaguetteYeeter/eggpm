#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* catstring(char* string1, char* string2) {
    char* result = (char *) malloc((strlen(string1) + strlen(string2) + 1) * sizeof(char));
    if (result == NULL) {
        exit(1);
    }

    strcpy(result, string1);
    strcat(result, string2);

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

void add_string_list(char** list, int* listc, char* string) {
    int count = *listc;
    char** result = realloc(list, sizeof(char*) * (count + 1));
    if (result == NULL) {
        exit(1);
    }

    result[count] = strdup(string);
    if (result[count] == NULL) {
        exit(1);
    }

    count++;
    *listc = count;
    list = result;
}

char** split_string(char *string, char *split, int *count) {
    char** result = (char**) malloc(sizeof(char*));
    int size = 0;

    char *token = strtok(string, split);
    while (token) {
        add_string_list(result, &size, token);
        token = strtok(NULL, split);
    }

    if (count != (int*) -1) {
        *count = size;
    }
    return result;
}