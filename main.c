#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "file_redirection.h"

void execute_command(char *args[], int parent_child_concurrent);

void copyStringArray(char *destination[], char *source[])
{
    int i = 0;
    while (source[i] != NULL)
    {
        destination[i] = strdup(source[i]);
        i++;
    }
    destination[i] = NULL; // Add NULL termination to the destination array
}

/*
    returns:
     0 for input redirection
     1 for output redirection
     -1 no redirection
*/

int main(void)
{
    char input[MAX_LINE];                // buffer to store user input
    char *argsHistory[MAX_LINE / 2 + 1]; /* histoy command line arguments */
    char *args[MAX_LINE / 2 + 1];        /* command line arguments */
    int should_run = 1;                  /* flag to determine when to exit program */
    int parent_child_concurrent = 0;     // flag to determine when parent and child should run concurrently
    int isHistoryEmpty = 0;              // flag to determine when history is empty
    while (should_run)
    {
        printf("oscz>");
        fflush(stdout);

        // read user input
        fgets(input, sizeof(input), stdin);
        // Remove the newline character if present
        if (input[strlen(input) - 1] == '\n')
            input[strlen(input) - 1] = '\0';
        /* Tokenize the input into command line arguments */
        tokenize(input, args, &parent_child_concurrent);

        if (strcmp(args[0], "!!") == 0)
        {
            if (isHistoryEmpty == 0)
            {
                printf("No commands in history.");
                fflush(stdout);
                continue;
            }
            else
            {
                copyStringArray(args, argsHistory);
            }
        }
        if (strcmp(args[0], "exit") == 0)
        {
            should_run = 0;
            exit(0);
        }
        else
        {
            copyStringArray(argsHistory, args);
            isHistoryEmpty = 1;
        }

        execute_command(args, parent_child_concurrent);
    }
    return 0;
}

void execute_command(char *args[], int parent_child_concurrent)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("error forking");
    }
    else if (pid == 0)
    {
        /* in child */
        struct file_redirection *file = (struct file_redirection *)malloc(sizeof(struct file_redirection));
        isRedirected(args, file);

        switch (file->redirection_type)
        {
        case INPUT_REDIRECTION:
            // input redirection
            int input_fd  = open(file->file_name, O_RDWR); // Open the source file in read-only mode
            if (input_fd == -1)
            {
                perror("Failed to open source file");
                exit(1);
            }
            dup2(input_fd , STDIN_FILENO);
            break;
        case OUTPUT_REDIRECTION:
            // output redirection
            int output_fd  = open(file->file_name, O_RDWR | O_CREAT, 0644); // Open the source file in read-only mode
            if (output_fd == -1)
            {
                perror("Failed to open source file");
                exit(1);
            }
            dup2(output_fd, STDOUT_FILENO);
            break;

        default:
            break;
        }

        execvp(args[0], args);
        perror("execvp error"); /* execvp() only returns if an error occurs */
        exit(1);
    }
    else
    {
        /* in parent */
        if (parent_child_concurrent == 1)
        {
            // do not wait for child to return
        }
        else
        {
            wait(NULL); // wait for the child to return
        }
    }
}


void tokenize(char input[MAX_LINE], char *args[MAX_LINE / 2 + 1], int *isConcurrent)
{
    int arg_count = 0;
    char *token = strtok(input, " \n");
    while (token != NULL)
    {
        if (strcmp(token, "&") == 0)
        {
            *isConcurrent = 1;
            args[arg_count] = NULL; /* Set the last element to NULL for execvp() */
            break;
        }
        else
        {
            // do not wait for child to return
            *isConcurrent = 0;
            args[arg_count++] = token;
        }
        token = strtok(NULL, " \n");
    }
}