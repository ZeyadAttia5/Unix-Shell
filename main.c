#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "file_redirection.h"

int main(void)
{
    char input[MAX_LINE];                // buffer to store user input
    char *argsHistory[MAX_LINE / 2 + 1]; /* histoy command line arguments */
    // char *args[MAX_LINE / 2 + 1];        /* command line arguments */
    // char *args2[MAX_LINE / 2 + 1];       /* command line arguments for the second command (pipe) */
    int should_run = 1;                  /* flag to determine when to exit program */
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

        struct command *command1 = (struct command *)malloc(sizeof(struct command));
        struct command *command2 = (struct command *)malloc(sizeof(struct command));

        /* Tokenize the input into command line arguments */
        tokenize(input, command1, command2);
        // copyStringArray(args, command1->args);
        // copyStringArray(args2, command2->args);

        if (strcmp(command1->args[0], "exit") == 0)
        {
            should_run = 0;
            exit(0);
        }
        else
        {
            copyStringArray(argsHistory, command1->args);
            isHistoryEmpty = 1;
        }

        execute_pipeLine(command1, command2);
        free(command1);
        free(command2);
    }
    return 0;
}
