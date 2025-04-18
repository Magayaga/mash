// sleep.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

void print_usage(const char *progname) {
    fprintf(stderr, "Usage: %s [duration][s|m|h]\n", progname);
    fprintf(stderr, "Examples:\n");
    fprintf(stderr, "  %s 5s   # 5 seconds\n", progname);
    fprintf(stderr, "  %s 2m   # 2 minutes\n", progname);
    fprintf(stderr, "  %s 1h   # 1 hour\n", progname);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }

    char *input = argv[1];
    size_t len = strlen(input);

    if (len < 2) {
        fprintf(stderr, "Invalid duration: %s\n", input);
        return 1;
    }

    char unit = tolower(input[len - 1]);
    input[len - 1] = '\0'; // Remove unit character

    char *endptr;
    double value = strtod(input, &endptr);

    if (*endptr != '\0' || value < 0) {
        fprintf(stderr, "Invalid time value: %s\n", argv[1]);
        return 1;
    }

    double seconds;
    switch (unit) {
        case 's':
            seconds = value;
            break;
        case 'm':
            seconds = value * 60.0;
            break;
        case 'h':
            seconds = value * 3600.0;
            break;
        default:
            fprintf(stderr, "Unknown time unit '%c'. Use 's', 'm', or 'h'.\n", unit);
            return 1;
    }

#ifdef _WIN32
    DWORD milliseconds = (DWORD)(seconds * 1000);
    Sleep(milliseconds);
#else
    unsigned int secs = (unsigned int)seconds;
    unsigned int microsecs = (seconds - secs) * 1000000;

    sleep(secs);
    usleep(microsecs);
#endif

    return 0;
}
