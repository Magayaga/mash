#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

void remove_directory(const char* dirname) {
    if (rmdir(dirname) == 0) {
        printf("Directory removed successfully.\n");
    }
    
    else {
        printf("Error: Unable to remove directory.\n");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <dirname>\n", argv[0]);
        return 1;
    }
    remove_directory(argv[1]);
    return 0;
}

