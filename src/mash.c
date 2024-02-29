#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include "color.c"

#define BUFFER_SIZE 1024
#define HISTORY_FILE ".mash_history"

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

void save_history_to_file(HistoryNode* head) {
    FILE* history_file = fopen(HISTORY_FILE, "w");
    if (history_file == NULL) {
        perror("Error opening history file for writing");
        return;
    }

    HistoryNode* current = head;
    while (current != NULL) {
        fprintf(history_file, "%s\n", current->command);
        current = current->next;
    }

    fclose(history_file);
}

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

void execute_command(char* command) {
    if (strncmp(command, "cat ", 4) == 0) {
        // Handle "cat" command
        char* filename = command + 4; // Skip "cat " to get the filename
        FILE* file = fopen(filename, "r");

        if (file) {
            char buffer[BUFFER_SIZE];
            while (fgets(buffer, BUFFER_SIZE, file) != NULL) {
                printf("%s", buffer);
            }
            fclose(file);
        }

        else {
            printf("Error: File not found or unable to open.\n");
        }
    }

    else if (strcmp(command, "ls ") == 0) {
        // Handle "ls" command
        DIR* dir = opendir(".");
        if (dir) {
             struct dirent* entry;
             while ((entry = readdir(dir)) != NULL) {
                   printf("%s\n", entry->d_name);
             }
             closedir(dir);
        }
        else {
             printf("Error: Unable to open directory.\n");
        }
    }

    else if (strncmp(command, "cp ", 3) == 0) {
        // Handle "cp" command
        char* src_filename = strtok(command + 3, " "); // Skip "cp " to get the source filename
        char* dest_filename = strtok(NULL, " "); // Get the destination filename

        FILE* src_file = fopen(src_filename, "r");
        FILE* dest_file = fopen(dest_filename, "w");

        if (src_file && dest_file) {
            char buffer[BUFFER_SIZE];
            size_t bytes_read;
            while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, src_file)) > 0) {
                fwrite(buffer, 1, bytes_read, dest_file);
            }
            fclose(src_file);
            fclose(dest_file);
            printf("File copied successfully.\n");
        }

            else {
            printf("Error: Unable to copy file.\n");
        }
    }

    else if (strncmp(command, "mv ", 3) == 0) {
        // Handle "mv" command
        char* src_filename = strtok(command + 3, " "); // Skip "mv " to get the source filename
        char* dest_filename = strtok(NULL, " "); // Get the destination filename

        if (rename(src_filename, dest_filename) == 0) {
            printf("File moved successfully.\n");
        }

            else {
            printf("Error: Unable to move file.\n");
        }
    }

    else if (strncmp(command, "rm ", 3) == 0) {
        // Handle "rm" command
        char* filename = command + 3; // Skip "rm " to get the filename

        if (remove(filename) == 0) {
            printf("File removed successfully.\n");
        }

            else {
            printf("Error: Unable to remove file.\n");
        }
    }

    else if (strncmp(command, "mkdir ", 6) == 0) {
        // Handle "mkdir" command
        char* dirname = command + 6; // Skip "mkdir " to get the directory name

        if (mkdir(dirname) == 0) {
            printf("Directory created successfully.\n");
        }

            else {
            printf("Error: Unable to create directory.\n");
        }
    }

    else if (strncmp(command, "rmdir ", 6) == 0) {
        // Handle "rmdir" command
        char* dirname = command + 6; // Skip "rmdir " to get the directory name

        if (rmdir(dirname) == 0) {
            printf("Directory removed successfully.\n");
        }

            else {
            printf("Error: Unable to remove directory.\n");
        }
    }

    else if (strncmp(command, "cd ", 3) == 0) {
        // Handle "cd" command
        char* dirname = command + 3; // Skip "cd " to get the directory name

        if (chdir(dirname) == 0) {
            printf("Changed directory to '%s'\n", dirname);
        }

            else {
            printf("Error: Unable to change directory.\n");
        }
    }

    else {
        // Other commands, execute using system()
        system(command);
    }
}

// Function to execute the command
void calculator_command(char* command, char* args[]) {
    // Add your custom commands here
    if (strcmp(command, "hello") == 0) {
        printf("Hello, World!\n");
    }

    else if (strcmp(command, "add") == 0) {
        if (args[0] != NULL && args[1] != NULL) {
            int a = atoi(args[0]);
            int b = atoi(args[1]);
            printf("Result: %d\n", a + b);
        }

            else {
            printf("Invalid arguments for add command.\n");
        }
    }

    else if (strcmp(command, "sub") == 0) {
        if (args[0] != NULL && args[1] != NULL) {
            int a = atoi(args[0]);
            int b = atoi(args[1]);
            printf("Result: %d\n", a - b);
        }

            else {
            printf("Invalid arguments for sub command.\n");
        }
    }

    else if (strcmp(command, "mul") == 0) {
        if (args[0] != NULL && args[1] != NULL) {
            int a = atoi(args[0]);
            int b = atoi(args[1]);
            printf("Result: %d\n", a * b);
        }

            else {
            printf("Invalid arguments for mul command.\n");
        }
    }

    else if (strcmp(command, "div") == 0) {
        if (args[0] != NULL && args[1] != NULL) {
            int a = atoi(args[0]);
            int b = atoi(args[1]);
            if (b != 0) {
                printf("Result: %.2f\n", (float)a / b);
            } else {
                printf("Error: Division by zero!\n");
            }
        }

        else {
            printf("Invalid arguments for div command.\n");
        }
    }

    else {
        printf("Unknown command: %s\n", command);
    }
}

void execute_sh_command(char* command) {
    FILE* fp;
    char buffer[BUFFER_SIZE];

    // Open a pipe to execute the command
    fp = popen(command, "r");
    if (fp == NULL) {
        printf("Error: Failed to execute command.\n");
        return;
    }

    // Read and print the output of the command
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }

    // Close the pipe
    pclose(fp);
}

void execute_time_command(char* command) {
    // Get the start time
    clock_t start_time = clock();

    // Execute the specified command
    execute_command(command);

    // Get the end time
    clock_t end_time = clock();

    // Calculate the execution time
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("\nExecution time: %f seconds\n", time_taken);
}

void execute_date_command() {
    // Execute the "date" command to display the current date and time
#ifdef _WIN32
    system("date /T");
#else
    system("date");
#endif
}

void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Function to execute non-built-in commands
void execute_history_command(char* command) {
    FILE* output_pipe = popen(command, "r");
    if (output_pipe == NULL) {
        printf("Error: Failed to execute command.\n");
    }

    else {
        // Capture the output of the command
        char command_output[BUFFER_SIZE];
        size_t bytes_read = fread(command_output, 1, sizeof(command_output) - 1, output_pipe);
        command_output[bytes_read] = '\0';
        printf("%s", command_output);
        pclose(output_pipe);
    }
}

int main(int argc, char* argv[]) {

    // Set up the Ctrl+C signal handler
    signal(SIGINT, handle_ctrlc);

    HistoryNode* history_head = load_history_from_file();

    if (argc == 2) {
       if (strcmp(argv[1], "--help") == 0) {
           // Display help information and exit
           printf("MASH (Magayaga Automation Shell)\n");
           printf("Available commands:\n");
           printf("  alias <name>=<command>  Define an alias for a command\n");
           printf("  cd <directory>          Change the current directory\n");
           printf("  sh                      Execute a shell command\n");
           printf("  time <command>          Time a simple command\n");
           printf("  date                    Display the current date and time\n");
           printf("  history                 Show command history\n");
           printf("  exit                    Exit the shell\n");
           return 0;
       }

       else if (strcmp(argv[1], "--version") == 0) {
           // Display version information and exit
           printf("MASH (Magayaga Automation Shell) | Version: v1.0-preview10\n");
           printf("Copyright (c) 2023 Cyril John Magayaga\n");
           printf("MIT License: <https://opensource.org/license/mit/>\n");
       }
    }

    char buffer[BUFFER_SIZE];
    char* named_directories[10]; // Maximum 10 named directories
    char* directory_paths[10];   // Corresponding paths for named directories
    int num_named_directories = 0;

    while (1) {
        // Print prompt with current directory
        char cwd[BUFFER_SIZE];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("\n%s\n@~ ", cwd);
        }

        else {
            printf("Error: Unable to get current directory.\n");
            break;
        }

        // Read input
        if (!fgets(buffer, BUFFER_SIZE, stdin)) {
            printf("\n");
            break;
        }
        // Remove newline character from input
        buffer[strcspn(buffer, "\n")] = '\0';

        // Check for named directory commands
        if (strncmp(buffer, "alias", 6) == 0) {
            // ... (rest of the alias command handling remains unchanged)
        }

        else if (strncmp(buffer, "cat", 4) == 0) {
            // Handle "cat" command
            printf("Usage: Usage: [OPTION]... [FILE]...\n");
        }

        else if (strcmp(buffer, "ls") == 0) {
            // Handle "ls" command
            DIR* dir = opendir(".");

            if (dir) {
               struct dirent* entry;
               while ((entry = readdir(dir)) != NULL) {
                   printf("%s\n", entry->d_name);
               }
               closedir(dir);
            }

            else {
               printf("Error: Unable to open directory.\n");
            }

        }

        else if (strncmp(buffer, "cd", 3) == 0) {
            // ... (rest of the cd command handling remains unchanged)
        }

        else if (strcmp(buffer, "sh") == 0) {
            // ... (rest of the sh command handling remains unchanged)
        }

        else if (strncmp(buffer, "time", 5) == 0) {
            // ... (rest of the time command handling remains unchanged)
        }

        else if (strcmp(buffer, "date") == 0) {
            // Handle "date" command to display the current date and time
            execute_date_command();
        }

        else if (strcmp(buffer, "clear") == 0) {
            // Handle "clear" command to clear the terminal screen
            clear_screen();
        }

        else if (strcmp(buffer, "mash --help") == 0) {
            printf("MASH (Magayaga Automation Shell)\n");
            printf("Available commands:\n");
            printf("  alias <name>=<command>  Define an alias for a command\n");
            printf("  cd <directory>          Change the current directory\n");
            printf("  sh                      Execute a shell command\n");
            printf("  time <command>          Time a simple command\n");
            printf("  date                    Display the current date and time\n");
            printf("  history                 Show command history\n");
            printf("  exit                    Exit the shell\n");
        }

        else if (strcmp(buffer, "mash --version") == 0) {
            printf("MASH (Magayaga Automation Shell) | Version: v1.0\n");
            printf("Copyright (c) 2023 Cyril John Magayaga\n");
            printf("MIT License: <https://opensource.org/license/mit/>\n");
        }

        else if (strcmp) {
            // Execute other commands
            execute_command(buffer);
        }

        else if (strcmp) {
            // Execute other calculator commands
            // calculator_command(command, argv);
        }
        else {
            // Execute other history commands
            execute_history_command(buffer);
        }
    }

    while (!exit_requested) {
        // ... (main loop remains unchanged)

        // Check for the "exit" command
        if (strcmp(buffer, "exit") == 0) {
            handle_exit_command();
        }

        else {
            // Add the command to history
            history_head = add_to_history(history_head, buffer);
        }
    }

    // Free memory used for named directories
    for (int i = 0; i < num_named_directories; i++) {
        free(named_directories[i]);
        free(directory_paths[i]);
    }

    // Free the history list
    free_history(history_head);

    return 0;
}
