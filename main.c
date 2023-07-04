#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "file_redirection.h"
#include "commands.h"

int main(void) {
    char input[MAX_LINE];                // buffer to store user input
    char *argsHistory[MAX_LINE / 2 + 1]; /* histoy command line arguments */
    char *args[MAX_LINE / 2 + 1];        /* command line arguments */
    char *args2[MAX_LINE / 2 + 1];       /* command line arguments for the second command (pipe) */
    int should_run = 1;                  /* flag to determine when to exit program */
    int parent_child_concurrent = 0;     // flag to determine when parent and child should run concurrently
    int isHistoryEmpty = 0;              // flag to determine when history is empty
    int isPipe = 0;                      // flag to determine when there is a pipe
    while (should_run) {
        printf("oscz>");
        fflush(stdout);

        // read user input
        fgets(input, sizeof(input), stdin);
        // Remove the newline character if present
        if (input[strlen(input) - 1] == '\n')
            input[strlen(input) - 1] = '\0';
        /* Tokenize the input into command line arguments */
        tokenize(input, args, args2, &parent_child_concurrent, &isPipe);

        if (strcmp(args[0], "exit") == 0) {
            should_run = 0;
            exit(0);
        } else if(strcmp(args[0], "!!") != 0) { //does not match
            copyStringArray(argsHistory, args);
            isHistoryEmpty = 1;
        }
        if (isHistoryEmpty == 1 && strcmp(args[0], "!!") == 0) {
            execute_pipeLine(argsHistory, args2, parent_child_concurrent, isPipe);
        } else if (isPipe && isHistoryEmpty && strcmp(args2[0], "!!") == 0) {
            execute_pipeLine(args, argsHistory, parent_child_concurrent, isPipe);
        } else {
            execute_pipeLine(args, args2, parent_child_concurrent, isPipe);
        }

    }
    return 0;
}

void execute_command(char *args[], int parent_child_concurrent, int fd_in, int fd_out) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("error forking");
    } else if (pid == 0) {
        /* in child */

        // input is from the pipe
        if (fd_in != STDIN_FILENO) {
            // redirect to STDIN
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        // output to the pipe
        if (fd_out != STDOUT_FILENO) {
            // redirect to STDOUT
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }
        struct file_redirection *file = (struct file_redirection *) malloc(sizeof(struct file_redirection));
        isRedirected(args, file);

        switch (file->redirection_type) {
            case INPUT_REDIRECTION:
                // input redirection
                int input_fd = open(file->file_name, O_RDWR); // Open the source file in read-only mode
                if (input_fd == -1) {
                    perror("Failed to open source file");
                    exit(1);
                }
                dup2(input_fd, STDIN_FILENO);
                close(input_fd);
                break;
            case OUTPUT_REDIRECTION:
                // output redirection
                int output_fd = open(file->file_name, O_WRONLY | O_CREAT,
                                     0644); // Open the source file in read-only mode
                if (output_fd == -1) {
                    perror("Failed to open source file");
                    exit(1);
                }
                dup2(output_fd, STDOUT_FILENO);
                close(output_fd);
                break;

            default:
                break;
        }

        clearFileRedirection(file);
        execvp(args[0], args);
        perror("execvp error"); /* execvp() only returns if an error occurs */
        exit(1);
    } else {
        /* in parent */
        if (parent_child_concurrent == 1) {
            // do not wait for child to return
        } else {
            wait(NULL); // wait for the child to return
        }
    }
}

void tokenize(char input[MAX_LINE], char *args[MAX_LINE / 2 + 1], char *args2[MAX_LINE / 2 + 1], int *isConcurrent,
              int *isPipe) {
    int arg_count = 0;
    *isConcurrent = 0;
    *isPipe = 0;
    char *token = strtok(input, " \n");
    while (token != NULL) {
        if (strcmp(token, "&") == 0) {
            *isConcurrent = 1;
            args[arg_count] = NULL; /* Set the last element to NULL for execvp() */
            break;
        } else if (strcmp(token, "|") == 0) {
            *isPipe = 1;
            token = strtok(NULL, " \n");
            tokenize(token, args2, NULL, isConcurrent, isPipe);
            break;
        } else {
            args[arg_count++] = token;
        }
        token = strtok(NULL, " \n");
    }
    if (!*isPipe) {
        args[arg_count] = NULL;
    }
}

void copyStringArray(char *destination[], char *source[]) {
    int i = 0;
    while (source[i] != NULL) {
        destination[i] = strdup(source[i]);
        i++;
    }
    destination[i] = NULL; // Add NULL termination to the destination array
}

void execute_pipeLine(char *args[], char *args2[], int isConcurrent, int isPipe) {

    if (isPipe == 1) {
        int pipefd[2];
        // create pipe
        if ((pipe(pipefd) == -1)) {
            perror("pipe error");
            exit(1);
        }
        // close read end
        close(pipefd[0]);

        // execute 1st command
        execute_command(args, isConcurrent, STDIN_FILENO, pipefd[1]);

        // close write end
        close(pipefd[1]);

        // execute 2nd command
        execute_command(args2, isConcurrent, pipefd[0], STDOUT_FILENO);
    } else {
        execute_command(args, isConcurrent, STDIN_FILENO, STDOUT_FILENO);
    }
}