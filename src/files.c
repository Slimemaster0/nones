#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include "files.h"

/// Returns the content of a file as a char* or a null pointer
struct binArray readFileBinary(char *path) {
    FILE *fptr = fopen(path, "rb");

    if (fptr == NULL) {
	struct binArray ret = { NULL, 0 };
	return ret;
    }

    size_t fileSize = getFileSize(path);

    char *contents = malloc(sizeof(char)*fileSize);
    fread(contents, 1, fileSize, fptr);
    fclose(fptr);

    struct binArray ret = { contents, sizeof(char)*fileSize };
    return ret;
}

size_t getFileSize(char *path) {
    FILE *fptr = fopen(path, "rb");
    if (fptr == NULL) {
	return -1;
    }

    fseek(fptr, 0, SEEK_END);
    size_t fileSize = ftell(fptr);
    fclose(fptr);
    return fileSize;
}
