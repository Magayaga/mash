#include <stdio.h>
#include <stdlib.h>

void mv(const char* src_filename, const char* dest_filename) {
    if (rename(src_filename, dest_filename) == 0) {
        printf("File moved successfully.\n");
    }
    
    else {
        printf("Error: Unable to move file.\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <source> <destination>\n", argv[0]);
        return 1;
    }
    mv(argv[1], argv[2]);
    return 0;
}

