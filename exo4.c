#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>

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
    char cmd[MAX_CMD_LEN];
    char *argv[MAX_ARGS];
    int argc;

    while (1) {
        printf("$ ");
        fflush(stdout);

        if (!fgets(cmd, MAX_CMD_LEN, stdin)) {
            perror("fgets");
            continue;
        }
        cmd[strcspn(cmd, "\n")] = 0;

        if (strcmp(cmd, "exit") == 0) {
            break;
        }

        argc = parse_line(cmd, argv);
        if (argc >= MAX_ARGS) {
            fprintf(stderr, "Error: Too many arguments.\n");
            continue;
        }

        int redirect = 0; 
        int fd = -1;     
        if (argc > 2 && strcmp(argv[argc-2], ">") == 0) {
            redirect = 1;
            argv[argc-2] = NULL;

            fd = open(argv[argc-1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                perror("open");
                continue;
            }
            argc -= 2; 
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            continue;
        } else if (pid == 0) {
            if (redirect) {
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            execvp(argv[0], argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else {
            wait(NULL);
            if (redirect) {
                close(fd); 
            }
        }
    }

    return 0;
}
