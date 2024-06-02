#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

void make_directory(const char* dirname) {
#ifdef _WIN32
    if (_mkdir(dirname) == 0) {
#else
    if (mkdir(dirname, 0777) == 0) {
#endif
        printf("Directory '%s' created successfully.\n", dirname);
    }
    
    else {
        perror("Error creating directory");
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <dirname>\n", argv[0]);
        return 1;
    }

    make_directory(argv[1]);
    return 0;
}

