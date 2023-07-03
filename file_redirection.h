#ifndef FILE_REDIRECTION

#include "commands.h"
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#define INPUT_REDIRECTION 0
#define OUTPUT_REDIRECTION 1
struct file_redirection
{
    int redirection_type;
    char *file_name;
};

void isRedirected(char *args[MAX_LINE / 2 + 1], struct file_redirection *fd);
void clearFileRedirection(struct file_redirection *file);

#endif