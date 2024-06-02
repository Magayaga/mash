#include <stdio.h>
#include <stdlib.h>
#define BUFFER_SIZE 1024

void cp(const char* src_filename, const char* dest_filename) {
    FILE* src_file = fopen(src_filename, "r");
    if (src_file == NULL) {
        printf("Error: Source file not found or unable to open.\n");
        return;
    }

    FILE* dest_file = fopen(dest_filename, "w");
    if (dest_file == NULL) {
        printf("Error: Destination file unable to open.\n");
        fclose(src_file);
        return;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, src_file)) > 0) {
        fwrite(buffer, 1, bytes_read, dest_file);
    }
    fclose(src_file);
    fclose(dest_file);
    printf("File copied successfully.\n");
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <source> <destination>\n", argv[0]);
        return 1;
    }
    cp(argv[1], argv[2]);
    return 0;
}

