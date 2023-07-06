#ifndef COMMANDS_HEADER
#define MAX_LINE 80 /* The maximum length command */

void execute_command(char *args[], char *args2[], int parent_child_concurrent, int *isPipe);

void execute_pipeLine(char *args[], char *args2[], int isConcurrent);



#endif