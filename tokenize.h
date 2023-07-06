//
// Created by zeyad on 7/6/23.
//

#ifndef UNIX_SHELL_TOKENIZE_H
#define UNIX_SHELL_TOKENIZE_H

#include "commands.h"
#include "stdio.h"

void tokenize(char input[MAX_LINE], char *args[MAX_LINE / 2 + 1], char *args2[MAX_LINE / 2 + 1], int *isConcurrent,
              int *isPipe);

void copyStringArray(char *[MAX_LINE / 2 + 1], char *[MAX_LINE / 2 + 1]);

#endif //UNIX_SHELL_TOKENIZE_H
