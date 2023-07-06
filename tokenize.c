//
// Created by zeyad on 7/6/23.
//
#include "tokenize.h"
#include <string.h>
#include "commands.h"

void tokenize(char input[MAX_LINE], char *args[MAX_LINE / 2 + 1], char *args2[MAX_LINE / 2 + 1], int *isConcurrent,
              int *isPipe) {
    int arg_count = 0;
    *isConcurrent = 0;
    *isPipe ^= 0;
    char *token = strtok(input, " \n");
    while (token != NULL) {
        if (strcmp(token, "&") == 0) {
            *isConcurrent = 1;
            args[arg_count] = NULL; /* Set the last element to NULL for execvp() */
            break;
        } else if (strcmp(token, "|") == 0) {
            *isPipe = 1;
            token = strtok(NULL, " \n");
            args[arg_count] = NULL;
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
