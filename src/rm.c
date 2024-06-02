#include <stdio.h>
#include <stdlib.h>

void rm(const char* filename) {
    if (remove(filename) == 0) {
        printf("File removed successfully.\n");
    }
    
    else {
        printf("Error: Unable to remove file.\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }
    rm(argv[1]);
    return 0;
}

