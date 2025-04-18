#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include "color.c"

#define BUFFER_SIZE 1024
#define HISTORY_FILE ".mash_history"
#define STARTUP_SCRIPT_ETC "etc/.mash"
#define STARTUP_SCRIPT_RC ".mashrc"
#define STARTUP_SCRIPT_PROFILE ".mash_profile"

// Check if the operating system is Windows
bool is_windows() {
#ifdef _WIN32
    return true;
#else
    return false;
#endif
}

// Flag to indicate if Ctrl+C was pressed
volatile sig_atomic_t ctrlc_pressed = 0;

// Flag to indicate if the "exit" command was issued
bool exit_requested = false;

// Structure to hold a command in the history
typedef struct HistoryNode {
    char command[BUFFER_SIZE];
    struct HistoryNode* next;
} HistoryNode;

// Function to add a command to the history
HistoryNode* add_to_history(HistoryNode* head, const char* command) {
    HistoryNode* new_node = (HistoryNode*)malloc(sizeof(HistoryNode));
    if (new_node == NULL) {
        printf("Error: Memory allocation failed.\n");
        return head;
    }

    strcpy(new_node->command, command);
    new_node->next = head;

    return new_node;
}

// Append a command to the history file
void append_history_to_file(const char* command) {
    FILE* history_file = fopen(HISTORY_FILE, "a");
    if (history_file == NULL) {
        perror("Error opening history file for appending");
        return;
    }

    fprintf(history_file, "%s\n", command);
    fclose(history_file);
}

// Load history from file
HistoryNode* load_history_from_file() {
    FILE* history_file = fopen(HISTORY_FILE, "r");
    if (history_file == NULL) {
        // If the history file doesn't exist yet, return NULL (empty history)
        return NULL;
    }

    HistoryNode* head = NULL;
    char buffer[BUFFER_SIZE];

    while (fgets(buffer, BUFFER_SIZE, history_file)) {
        // Remove newline character from the command before adding to history
        buffer[strcspn(buffer, "\n")] = '\0';
        head = add_to_history(head, buffer);
    }

    fclose(history_file);
    return head;
}

// Function to free the history list
void free_history(HistoryNode* head) {
    while (head != NULL) {
        HistoryNode* temp = head;
        head = head->next;
        free(temp);
    }
}

// Signal handler for Ctrl+C (SIGINT)
void handle_ctrlc(int signum) {
    printf("\nCtrl+C pressed. Use 'exit' to exit the shell.\n");
}

// Function to handle the "exit" command
void handle_exit_command() {
    printf("Exiting the shell. Perform any cleanup if needed.\n");
    exit_requested = true;
}

// Execute a command
void execute_command(char* command) {
    char* args[BUFFER_SIZE];
    int i = 0;

    // Tokenize the input command
    char* token = strtok(command, " ");
    while (token != NULL) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    if (args[0] == NULL) {
        return; // No command entered
    }

    if (strcmp(args[0], "exit") == 0) {
        handle_exit_command();
    }
    
    else if (strcmp(args[0], "cat") == 0) {
        if (args[1] != NULL) {
            char command[BUFFER_SIZE];
            snprintf(command, sizeof(command), "./cat %s", args[1]);
            system(command);
        }
        
        else {
            printf("Usage: cat <filename>\n");
        }
    }
    else if (strncmp(command, "ls", 2) == 0) {
        // Handle "ls" command
        char ls_command[BUFFER_SIZE];
        snprintf(ls_command, sizeof(ls_command), "./ls %s", command + 3);
        system(ls_command);
    }

    else if (strcmp(args[0], "cp") == 0) {
        if (args[1] != NULL && args[2] != NULL) {
            char command[BUFFER_SIZE];
            snprintf(command, sizeof(command), "./cp %s %s", args[1], args[2]);
            system(command);
        }
        else {
            printf("Usage: cp <source> <destination>\n");
        }
    }
    
    else if (strcmp(args[0], "mv") == 0) {
        if (args[1] != NULL && args[2] != NULL) {
            char command[BUFFER_SIZE];
            snprintf(command, sizeof(command), "./mv %s %s", args[1], args[2]);
            system(command);
        }
        else {
            printf("Usage: mv <source> <destination>\n");
        }
    }
    
    else if (strcmp(args[0], "rm") == 0) {
        if (args[1] != NULL) {
            char command[BUFFER_SIZE];
            snprintf(command, sizeof(command), "./rm %s", args[1]);
            system(command);
        }
        
        else {
            printf("Usage: rm <filename>\n");
        }
    }
    
    else if (strcmp(args[0], "mkdir") == 0) {
        if (args[1] != NULL) {
            char command[BUFFER_SIZE];
            snprintf(command, sizeof(command), "./mkdir %s", args[1]);
            system(command);
        }
        
        else {
            printf("Usage: mkdir <dirname>\n");
        }
    }
    else if (strcmp(args[0], "rmdir") == 0) {
        if (args[1] != NULL) {
            char command[BUFFER_SIZE];
            snprintf(command, sizeof(command), "./rmdir %s", args[1]);
            system(command);
        }
        
        else {
            printf("Usage: rmdir <dirname>\n");
        }
    }
    else if (strcmp(args[0], "cd") == 0) {
        if (args[1] != NULL) {
            if (chdir(args[1]) == 0) {
                printf("Changed directory to '%s'\n", args[1]);
            }
            else {
                printf("Error: Unable to change directory.\n");
            }
        }
        else {
            printf("Usage: cd <dirname>\n");
        }
    }
    else {
        char buffer[BUFFER_SIZE];
        FILE* fp;

        // Open the pipe
        fp = popen(command, "r");
        if (fp == NULL) {
            printf("Failed to run command\n");
            return;
        }

        // Read the output of the command
        while (fgets(buffer, BUFFER_SIZE, fp) != NULL) {
            printf("%s", buffer);
        }

        // Close the pipe
        pclose(fp);
    }
}

// Execute a Python command
void execute_python_command(char* command) {
    char command_buffer[BUFFER_SIZE + 8];
    snprintf(command_buffer, sizeof(command_buffer), "python -c \"%s\"", command);
    execute_command(command_buffer);
}

// Run a startup or shutdown script if it exists
void load_startup_script(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        // It's okay if the file doesn't exist, just skip
        return;
    }

    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), file)) {
        // Remove newline
        line[strcspn(line, "\n")] = '\0';

        if (strlen(line) > 0) {
            printf("Running startup command: %s\n", line);
            execute_command(line);
        }
    }

    fclose(file);
}

int main() {
    char input[BUFFER_SIZE];
    HistoryNode* history = load_history_from_file();

    // Load startup scripts
    load_startup_script(STARTUP_SCRIPT_ETC);
    load_startup_script(STARTUP_SCRIPT_RC);
    load_startup_script(STARTUP_SCRIPT_PROFILE);

    // Register the signal handler for SIGINT (Ctrl+C)
    signal(SIGINT, handle_ctrlc);

    // Load the command history from file
    history = load_history_from_file();

    while (!exit_requested) {
        printf("shell> ");
        if (fgets(input, BUFFER_SIZE, stdin) == NULL) {
            break;
        }

        // Remove newline character from the input
        input[strcspn(input, "\n")] = '\0';

        // Add the command to history
        history = add_to_history(history, input);

        // Append the command to the history file
        append_history_to_file(input);

        // Execute the command
        execute_command(input);
    }

    // Free the command history
    free_history(history);

    return 0;
}

