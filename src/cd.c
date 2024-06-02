#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void change_directory(const char* dirname) {
    if (chdir(dirname) == 0) {
        printf("Changed directory to '%s'\n", dirname);
    }
    
    else {
        printf("Error: Unable to change directory.\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <dirname>\n", argv[0]);
        return 1;
    }
    change_directory(argv[1]);
    return 0;
}

