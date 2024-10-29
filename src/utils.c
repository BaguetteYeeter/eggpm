#include <unistd.h>
#include <stdlib.h>
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