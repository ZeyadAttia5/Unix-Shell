#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "file_redirection.h"
#include "commands.h"


#define READ_END 0
#define WRITE_END 1

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
        printf("oscz> ");
        fflush(stdout);
        fflush(stdin);

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
        } else if (strcmp(args[0], "!!") == 0 && isHistoryEmpty == 1) {
            execute_command(argsHistory, args2, parent_child_concurrent, &isPipe);
        } else if (strcmp(args[0], "\n") != 0) {
            copyStringArray(argsHistory, args);
            isHistoryEmpty = 1;
            execute_command(args, args2, parent_child_concurrent, &isPipe);
        }
    }
    return 0;
}

void execute_command(char *args[], char *args2[], int parent_child_concurrent, int *isPipe) {

    pid_t pid1, pid2;
    int status1, status2;
    pid1 = fork();
    if (pid1 < 0) {
        perror("error forking in execute command");
        exit(1);
    } else if (pid1 == 0) {
        /* in child */

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
        if (*isPipe != 0) {
            *isPipe = 0;
            execute_pipeLine(args, args2, parent_child_concurrent);
        } else {
            execvp(args[0], args);
            perror("execvp error"); /* execvp() only returns if an error occurs */
            exit(1);
        }
    } else {

        /* in parent */
        if (parent_child_concurrent == 1) {
            // do not wait for child to return
        } else {
            wait(NULL); // wait for the child to return
        }
    }
}

void execute_pipeLine(char *args[], char *args2[], int isConcurrent) {
    int pipefd[2];
    pid_t pid1, pid2;
    int status1, status2;
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pid1 = fork();

    if (pid1 < 0) {
        perror("error forking in execute pipe line");
        exit(1);
    } else if (pid1 == 0) {
        // in child
        close(pipefd[READ_END]);
        dup2(pipefd[WRITE_END], STDOUT_FILENO);
        close(pipefd[WRITE_END]);
        execvp(args[0], args);
        perror("execvp error");
        //* execvp() only returns if an error occurs *//*
                exit(1);
    } else {
        // in parent
        pid2 = fork();
        if (pid2 < 0) {
            perror("error forking in execute pipe line");
            exit(1);
        } else if (pid2 == 0) {
            // in child
            close(pipefd[WRITE_END]);
            dup2(pipefd[READ_END], STDIN_FILENO);
            close(pipefd[READ_END]);
            execvp(args2[0], args2);
            perror("execvp error");
            //* execvp() only returns if an error occurs *//*
                    exit(1);
        } else {
            // in parent
            close(pipefd[READ_END]);
            close(pipefd[WRITE_END]);
            if (isConcurrent == 1) {
                // do not wait for children to return
            } else {
                waitpid(pid1, &status1, 0);
                waitpid(pid2, &status2, 0);
            }
        }
    }
}