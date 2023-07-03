#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "commands.h"

void tokenize(char input[MAX_LINE], struct command *command1, struct command *command2)
{
    int arg_count = 0;
    char *token = strtok(input, " \n");
    while (token != NULL)
    {
        if (strcmp(token, "&") == 0)
        {
            command1->args[arg_count] = NULL; /* Set the last element to NULL for execvp() */
            command1->isConcurrent = 1;
            break;
        }
        else if (strcmp(token, "|") == 0)
        {
            command1->isPipe = 1;
            token = strtok(NULL, " \n");
            command1->args[arg_count] = NULL; /* Terminate the args array before recursive call */
            tokenize(token, command2, command1);
            break;
        }
        else
        {
            command1->args[arg_count++] = token;
        }
        token = strtok(NULL, " \n");
    }
    if (!command1->isPipe)
    {
        command1->args[arg_count] = NULL; /* Set the last element to NULL for execvp() */
    }
}

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