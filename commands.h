#ifndef COMMANDS_HEADER
#define MAX_LINE 80 /* The maximum length command */

void tokenize(char input[MAX_LINE], char *args[MAX_LINE / 2 + 1], int *isConcurrent);
#endif