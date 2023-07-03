#ifndef COMMANDS_HEADER
#define MAX_LINE 80 /* The maximum length command */

struct command
{
    char *args[MAX_LINE / 2 + 1];
    char isConcurrent;
    char isPipe;
};

void execute_command(struct command *command1, int fd_in, int fd_out);
void execute_pipeLine(struct command *command1, struct command *command2);
void copyStringArray(char *destination[MAX_LINE / 2 + 1], char *source[MAX_LINE / 2 + 1]);
void tokenize(char input[MAX_LINE], struct command *command1, struct command *command2);
#endif