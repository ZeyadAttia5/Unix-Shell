#ifndef COMMANDS_HEADER
#define MAX_LINE 80 /* The maximum length command */

void tokenize(char input[MAX_LINE], char *args[MAX_LINE / 2 + 1], char *args2[MAX_LINE / 2 + 1], int *isConcurrent, int *isPipe);
void execute_command(char *args[], int parent_child_concurrent);
void execute_pipeLine(char *args[], char *args2[], int isConcurrent, int isPipe);
void copyStringArray(char * [MAX_LINE / 2 + 1], char * [MAX_LINE / 2 + 1]);
#endif