#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE 1024
#define MAX_FIELDS 128
#define MAX_ACTION 1024

char *fields[MAX_FIELDS];
int field_count = 0;
long line_number = 0;

// Split a line into fields
void split_fields(char *line) {
    field_count = 0;
    char *token = strtok(line, " \t\n");
    while (token != NULL && field_count < MAX_FIELDS) {
        fields[++field_count] = token; // Start fields[1] like AWK
        token = strtok(NULL, " \t\n");
    }
}

// Execute simple action: currently supports only "print" command
void execute_action(const char *action) {
    if (strncmp(action, "print", 5) == 0) {
        const char *p = action + 5;
        while (isspace(*p)) p++; // skip whitespace

        while (*p) {
            if (*p == '$') {
                p++;
                int field_num = atoi(p);
                if (field_num >= 1 && field_num <= field_count) {
                    printf("%s", fields[field_num]);
                }
                while (isdigit(*p)) p++; // skip number
            } else if (*p == '"') {
                p++;
                while (*p && *p != '"') {
                    putchar(*p++);
                }
                if (*p == '"') p++;
            } else {
                putchar(*p++);
            }
        }
        printf("\n");
    }
}

// Match pattern
int match_pattern(const char *pattern) {
    if (!pattern || pattern[0] == '\0') return 1; // always match
    for (int i = 1; i <= field_count; ++i) {
        if (strstr(fields[i], pattern)) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s 'pattern' '{ action }' < input.txt\n", argv[0]);
        return 1;
    }

    const char *pattern = argv[1];
    const char *action = argv[2];
    char line[MAX_LINE];

    while (fgets(line, sizeof(line), stdin)) {
        line_number++;
        split_fields(line);
        if (match_pattern(pattern)) {
            execute_action(action);
        }
    }

    return 0;
}
