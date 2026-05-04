#include <stddef.h>

struct binArray readFileBinary(char *path);
size_t getFileSize(char *path);

struct binArray {
    char* contents;
    size_t size;
};
