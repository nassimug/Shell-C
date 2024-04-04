#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
#include <stdlib.h>

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

        while (*s != '\0' && !isspace((unsigned char)*s)) s++;

        if (*s != '\0') {
            *s = '\0';
            s++;
        }

        if (argc >= MAX_ARGS) {
            break;
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

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            continue;
        } else if (pid == 0) {
            execvp(argv[0], argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else {
            wait(NULL);
        }
    }

    return 0;
}
