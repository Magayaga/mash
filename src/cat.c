#include <stdio.h>
#include <stdlib.h>
#define BUFFER_SIZE 1024

void cat(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
        if (fputs(buffer, stdout) == EOF) {
            perror("Error writing to stdout");
            fclose(file);
            return;
        }
    }

    if (ferror(file)) {
        perror("Error reading from file");
    }

    if (fclose(file) == EOF) {
        perror("Error closing file");
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename> [...]\n", argv[0]);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        cat(argv[i]);
    }

    return 0;
}
