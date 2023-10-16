#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "file_redirection.h"
#include "commands.h"
#include "tokenize.h"



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
