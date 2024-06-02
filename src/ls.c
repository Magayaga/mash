#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <time.h>

void print_file_details(const char* path, const struct dirent* entry) {
    struct stat file_stat;
    char fullpath[1024];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

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

    // Print file permissions
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

    // Print file size
    printf(" %ld", file_stat.st_size);

    // Print last modification time
    char timebuf[80];
    struct tm* timeinfo;
    timeinfo = localtime(&file_stat.st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", timeinfo);
    printf(" %s", timebuf);

    // Print file name
    printf(" %s\n", entry->d_name);
}

void list_directory(const char* path, int show_all, int show_long) {
    DIR* dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!show_all && entry->d_name[0] == '.') {
            continue;
        }

        if (show_long) {
            print_file_details(path, entry);
        }
        
        else {
            printf("%s\n", entry->d_name);
        }
    }

    closedir(dir);
}

int main(int argc, char* argv[]) {
    const char* path = ".";
    int show_all = 0;
    int show_long = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0) {
            show_all = 1;
        }
        
        else if (strcmp(argv[i], "-l") == 0) {
            show_long = 1;
        }
        
        else if (strcmp(argv[i], "-la") == 0 || strcmp(argv[i], "-al") == 0) {
            show_all = 1;
            show_long = 1;
        }
        
        else {
            path = argv[i];
        }
    }

    list_directory(path, show_all, show_long);
    return 0;
}

