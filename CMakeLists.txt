PROJECT(Unix-Shell)

SET(C11)

add_executable(Unix-Shell main.c
        tokenize.c
        file_redirection.c
        tokenize.h
        file_redirection.h)