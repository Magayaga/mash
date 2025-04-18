// echo.c
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void print_with_escapes(const char *str) {
    for (const char *p = str; *p != '\0'; ++p) {
        if (*p == '\\') {
            ++p;
            switch (*p) {
                case 'n': putchar('\n'); break;
                case 't': putchar('\t'); break;
                case '\\': putchar('\\'); break;
                case 'r': putchar('\r'); break;
                case 'b': putchar('\b'); break;
                case 'f': putchar('\f'); break;
                case 'v': putchar('\v'); break;
                case 'a': putchar('\a'); break;
                default:
                    putchar('\\');
                    if (*p) putchar(*p);
                    break;
            }
        } else {
            putchar(*p);
        }
    }
}

int main(int argc, char *argv[]) {
    bool no_newline = false;
    int start = 1;

    if (argc > 1 && strcmp(argv[1], "-n") == 0) {
        no_newline = true;
        start = 2;
    }

    for (int i = start; i < argc; ++i) {
        print_with_escapes(argv[i]);
        if (i < argc - 1) {
            putchar(' ');
        }
    }

    if (!no_newline) {
        putchar('\n');
    }

    return 0;
}
