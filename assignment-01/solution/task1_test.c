#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <solution.h>

int main(int argc, char* argv[]) {
    uint64_t offset;
    char* filename;

    // Check CLI arguments
    if (argc != 3) {
        printf("Usage: %s <filepath> <offset>\n", argv[0]);
        return EXIT_FAILURE;
    }

    filename = argv[1];
    offset = atoi(argv[2]);

    // From Task description:
    // >  In particular, you can assume that offset is valid and does not point beyond the end of the file.
    // So we just check for input values less than zero. 
    if (offset <= 0) {
        printf("Offset must be a value greater zero!\n");
        return EXIT_FAILURE;
    } 

    // Simple usage of map
    char *ptr = map(filename, offset);
    if (!ptr) {
        printf("Mapping failed!\n");
        return EXIT_FAILURE;
    }

    printf("Byte at offset %ld: %c\n", offset, *ptr);

    return EXIT_SUCCESS;
}