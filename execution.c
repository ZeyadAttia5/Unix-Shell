
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "file_redirection.h"

void execute_command(struct command *command_generic, int fd_in, int fd_out)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("error forking");
    }
    else if (pid == 0)
    {
        /* in child */

        // input is from the pipe
        if (fd_in != STDIN_FILENO)
        {
            // redirect to STDIN
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }

        // output to the pipe
        if (fd_out != STDOUT_FILENO)
        {
            // redirect to STDOUT
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }
        struct file_redirection *file = (struct file_redirection *)malloc(sizeof(struct file_redirection));
        isRedirected(command_generic->args, file);

        switch (file->redirection_type)
        {
        case INPUT_REDIRECTION:
            // input redirection
            int input_fd = open(file->file_name, O_RDWR); // Open the source file in read-only mode
            if (input_fd == -1)
            {
                perror("Failed to open source file");
                exit(1);
            }
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
            break;
        case OUTPUT_REDIRECTION:
            // output redirection
            int output_fd = open(file->file_name, O_WRONLY | O_CREAT, 0644); // Open the source file in read-only mode
            if (output_fd == -1)
            {
                perror("Failed to open source file");
                exit(1);
            }
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
            break;

        default:
            break;
        }

        clearFileRedirection(file);
        execvp(command_generic->args[0], command_generic->args);
        perror("execvp error"); /* execvp() only returns if an error occurs */
        exit(1);
    }
    else
    {
        /* in parent */
        if (command_generic->isConcurrent == 1)
        {
            // do not wait for child to return
        }
        else
        {
            wait(NULL); // wait for the child to return
        }
    }
}

void execute_pipeLine(struct command *command1, struct command *command2)
{

    if (command1->isPipe == 1)
    {
        int pipefd[2];
        // create pipe
        if ((pipe(pipefd) == -1))
        {
            perror("pipe error");
            exit(1);
        }
        // Close the write end of the pipe
        close(pipefd[0]);
        // Execute the first command
        execute_command(command1, STDIN_FILENO, pipefd[1]);


        close(pipefd[1]);

        // Redirect the standard input of the second command to be the read end of the pipe
        dup2(pipefd[0], STDIN_FILENO);

        // Close the read end of the pipe
        // Execute the second command
        execute_command(command2, pipefd[0], STDOUT_FILENO);

    }
    else
    {
        execute_command(command1, STDIN_FILENO, STDOUT_FILENO);
    }
}