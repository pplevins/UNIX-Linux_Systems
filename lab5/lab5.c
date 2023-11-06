//Pinchas Plevinski
//322211558

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARG_SIZE 64

// Function to tokenize input into an array of arguments
void tokenizeInput(char *input, char *args[]) {
    char *token;
    int i = 0;

    // Tokenize the input using space as a delimiter
    token = strtok(input, " \n");
    while (token != NULL && i < MAX_ARG_SIZE - 1) {
        args[i++] = token;
        token = strtok(NULL, " \n");
    }
    args[i] = NULL; // Null-terminate the argument list
}

int main() {
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARG_SIZE];
    char *path = getenv("PATH");

    if (path == NULL) {
        perror("Failed to get PATH environment variable");
        return 1;
    }

    while (1) {
        printf("mini-shell> ");

        // Read user input
        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("Failed to read input");
            return 1;
        }

        // Check if the user wants to quit
        if (strcmp(input, "Quit\n") == 0) {
            break;
        }

        // Tokenize the input
        tokenizeInput(input, args);

        if (args[0] == NULL) {
            // Empty input, just show the prompt again
            continue;
        }

        // Fork a child process
        pid_t pid = fork();

        if (pid == -1) {
            perror("Failed to fork");
            return 1;
        }

        if (pid == 0) { // Child process
            // Search for the command in the directories listed in PATH
            char command[MAX_INPUT_SIZE];
            int found = 0;

            char *pathCopy = strdup(path);
            char *pathToken = strtok(pathCopy, ":");

            while (pathToken != NULL) {
                snprintf(command, sizeof(command), "%s/%s", pathToken, args[0]);
                if (access(command, X_OK) == 0) {
                    found = 1;
                    break;
                }
                pathToken = strtok(NULL, ":");
            }

            free(pathCopy);

            if (!found) {
                printf("Command not found: %s\n", args[0]);
                exit(1);
            }

            // Execute the command
            execv(command, args);

            // If execv fails, perror will be called
            perror("Failed to execute command");
            exit(1);
        }
	else { // Parent process
            // Wait for the child process to complete
            waitpid(pid, NULL, 0);
        }
    }
    return 0;
}
