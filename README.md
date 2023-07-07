# Shell Interface Project

This project consists of designing a C program to serve as a shell interface that accepts user commands and then
executes each command in a separate process. This implementation supports input and output redirection, as well as pipes
as a form of IPC between a pair of commands. Completing this project will involve using the
UNIX `fork()`, `exec()`, `wait()`, `dup2()`, and `pipe()` system calls and can be completed on any Linux, UNIX, or macOS
system.

## Project Objectives

- Design a shell interface program that provides a command-line prompt.
- Parse user input and execute commands in separate child processes.
- Implement support for input and output redirection.
- Enable inter-process communication using pipes.

## Getting Started

To get started with the project, follow these steps:

1. Clone the repository to your local machine.
2. Open the project in your preferred development environment.
3. Review the source code and documentation for a better understanding of the project structure and functionality.
4. Compile the program using a compatible C compiler (like gcc).
5. Run the compiled executable to start the shell interface.
6. Enter commands at the prompt and observe the program's behavior.

## Requirements

- C compiler (GCC recommended)
- Linux, UNIX, or macOS operating system

## Usage

1. Open a terminal window and navigate to the project directory.
2. Compile the source code using the following command: `gcc main.c -o main`
3. Run the compiled program: `./main`
4. The shell prompt will appear as `oscz>`. Enter commands and press Enter to execute them.
5. For commands requiring input or output redirection, use the appropriate symbols (`<`, `>`) followed by filenames.
6. To run commands concurrently, append `&` at the end of the command.
7. To use pipes, separate the first and second commands with a `|`   Ex: `ls -l | less`
8. To exit the shell, enter the command `exit`.

