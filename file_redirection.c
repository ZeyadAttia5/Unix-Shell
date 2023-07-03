#include "file_redirection.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void isRedirected(char *args[MAX_LINE / 2 + 1], struct file_redirection *fd)
{
    fd->file_name = '\0';
    fd->redirection_type = -1;

    int i = 0;
    while (args[i] != NULL)
    {
        if (strcmp(args[i], "<") == 0)
        {
            // copy name of file to input_file
            fd->file_name = (char *) malloc(sizeof(args[i+1]));
            strcpy(fd->file_name, args[i + 1]);
            fd->redirection_type = INPUT_REDIRECTION;

            // remove file name and input-redirection command
            args[i + 1] = NULL;
            args[i] = NULL;
        }
        else if (strcmp(args[i], ">") == 0)
        {
            // copy name of file to output_file
            fd->file_name = (char *) malloc(sizeof(args[i+1]));
            strcpy(fd->file_name, args[i + 1]);
            fd->redirection_type = OUTPUT_REDIRECTION;

            // remove file name and input-redirection command
            args[i + 1] = NULL;
            args[i] = NULL;
        }
        i++;
    }
}

void clearFileRedirection(struct file_redirection *file){
    free(file->file_name);
    free(file);
}