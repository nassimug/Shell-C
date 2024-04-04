#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h> 

#define MAX_CMD_LEN 1024
#define MAX_ARGS 17 

int parse_line(char *s, char **argv) {
    int argc = 0;

    while (*s != '\0') {
        while (isspace((unsigned char)*s)) s++;

        if (*s == '\0') {
            break;
        }

        argv[argc++] = s;

        if (argc >= MAX_ARGS) {
            break;
        }

        while (*s != '\0' && !isspace((unsigned char)*s)) s++;

        if (*s != '\0') {
            *s++ = '\0';
        }
    }

    argv[argc] = NULL;
    return argc;
}

int main() {
    char cmd1[MAX_CMD_LEN], cmd2[MAX_CMD_LEN];
    char *argv1[MAX_ARGS], *argv2[MAX_ARGS];
    int pipefd[2];
    int argc1, argc2;

    while (1) {
        printf("$ ");
        fflush(stdout);

        if (!fgets(cmd1, MAX_CMD_LEN, stdin)) {
            perror("fgets");
            continue;
        }
        cmd1[strcspn(cmd1, "\n")] = 0;

        if (strcmp(cmd1, "exit") == 0) {
            break;
        }

        argc1 = parse_line(cmd1, argv1);
        if (argc1 >= MAX_ARGS) {
            fprintf(stderr, "Error: Too many arguments.\n");
            continue;
        }

        if (argc1 > 1 && strcmp(argv1[argc1 - 1], "|") == 0) {
            argv1[argc1 - 1] = NULL; 

            printf("> ");
            fflush(stdout);

            if (!fgets(cmd2, MAX_CMD_LEN, stdin)) {
                perror("fgets");
                continue;
            }
            cmd2[strcspn(cmd2, "\n")] = 0;

            argc2 = parse_line(cmd2, argv2);
            if (argc2 >= MAX_ARGS) {
                fprintf(stderr, "Error: Too many arguments.\n");
                continue;
            }

            if (pipe(pipefd) == -1) {
                perror("pipe");
                continue;
            }

            if (fork() == 0) {
                close(pipefd[0]); 
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);

                execvp(argv1[0], argv1);
                perror("execvp");
                exit(EXIT_FAILURE);
            }

            if (fork() == 0) {
                close(pipefd[1]); 
                dup2(pipefd[0], STDIN_FILENO);
                close(pipefd[0]);

                execvp(argv2[0], argv2);
                perror("execvp");
                exit(EXIT_FAILURE);
            }

            close(pipefd[0]);
            close(pipefd[1]);
            wait(NULL);
            wait(NULL);
        } else {
            if (fork() == 0) {
                execvp(argv1[0], argv1);
                perror("execvp");
                exit(EXIT_FAILURE);
            }
            wait(NULL);
        }
    }

    return 0;
}
