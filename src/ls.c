#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

#define MAX_FILES 4096

void print_file_details(const char* path, const char* name);
int cmpstringp(const void* p1, const void* p2);

void list_directory(const char* path, int show_all, int show_long) {
    DIR* dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    struct dirent* entry;
    char* files[MAX_FILES];
    int count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (!show_all && entry->d_name[0] == '.') {
            continue;
        }
        files[count] = strdup(entry->d_name); // Save file name
        count++;
        if (count >= MAX_FILES) {
            fprintf(stderr, "Too many files\n");
            break;
        }
    }
    closedir(dir);

    // Sort filenames
    qsort(files, count, sizeof(char*), cmpstringp);

    // Print filenames
    for (int i = 0; i < count; i++) {
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, files[i]);

        if (show_long) {
            print_file_details(fullpath, files[i]);
        } else {
            printf("%s  ", files[i]);
        }
        free(files[i]); // clean up memory
    }

    if (!show_long) {
        printf("\n");
    }
}

// Compare function for qsort
int cmpstringp(const void* p1, const void* p2) {
    return strcmp(*(const char**)p1, *(const char**)p2);
}

void print_file_details(const char* path, const char* name) {
    struct stat file_stat;
    char fullpath[1024];
    snprintf(fullpath, sizeof(fullpath), "%s", path);

    if (stat(fullpath, &file_stat) != 0) {
        perror("stat");
        return;
    }

    // Print file type
    if (S_ISDIR(file_stat.st_mode)) {
        printf("d");
    }

    else if (S_ISREG(file_stat.st_mode)) {
        printf("-");
    }
    
    else {
        printf("?");
    }

    // Print permissions
    printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat.st_mode & S_IXUSR) ? "x" : "-");
    printf((file_stat.st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat.st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat.st_mode & S_IXGRP) ? "x" : "-");
    printf((file_stat.st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat.st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat.st_mode & S_IXOTH) ? "x" : "-");

    // Print number of hard links
    printf(" %ld", file_stat.st_nlink);

    // Print size
    printf(" %ld", file_stat.st_size);

    // Print last modified time
    char timebuf[80];
    struct tm* timeinfo;
    timeinfo = localtime(&file_stat.st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", timeinfo);
    printf(" %s", timebuf);

    // Print file name
    printf(" %s\n", name);
}

int main(int argc, char* argv[]) {
    int show_all = 0;
    int show_long = 0;
    int path_start = 1; // index where paths start

    // First, process flags (-a, -l, etc.)
    while (path_start < argc && argv[path_start][0] == '-') {
        if (strcmp(argv[path_start], "-a") == 0) {
            show_all = 1;
        }
        else if (strcmp(argv[path_start], "-l") == 0) {
            show_long = 1;
        }
        else if (strcmp(argv[path_start], "-la") == 0 || strcmp(argv[path_start], "-al") == 0) {
            show_all = 1;
            show_long = 1;
        }
        else {
            fprintf(stderr, "Unknown option: %s\n", argv[path_start]);
            return 1;
        }
        path_start++;
    }

    // If no path given, default to current directory
    if (path_start == argc) {
        list_directory(".", show_all, show_long);
    }
    else {
        for (int i = path_start; i < argc; i++) {
            struct stat path_stat;
            if (stat(argv[i], &path_stat) != 0) {
                perror(argv[i]);
                continue;
            }

            if (S_ISDIR(path_stat.st_mode)) {
                printf("%s:\n", argv[i]);
                list_directory(argv[i], show_all, show_long);
            }

            else {
                // It's a file, not a directory
                if (show_long) {
                    print_file_details(argv[i], argv[i]);
                }
                
                else {
                    printf("%s\n", argv[i]);
                }
            }

            if (i < argc - 1) {
                printf("\n"); // Separate multiple outputs
            }
        }
    }

    return 0;
}

