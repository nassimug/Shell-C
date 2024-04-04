#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
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
            *s = '\0';
            s++;
        }
    }

    argv[argc] = NULL;
    return argc;
}

pid_t last_child_pid = 0;

void sigint_handler(int sig) {
    if (last_child_pid > 0) {
        kill(last_child_pid, SIGINT);
    }
}

int main() {
    char cmd[MAX_CMD_LEN];
    char *argv[MAX_ARGS];
    int argc;
    struct sigaction sa;
    sigset_t new_mask, old_mask;

    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    sigemptyset(&new_mask);
    sigaddset(&new_mask, SIGINT);
    sigprocmask(SIG_BLOCK, &new_mask, &old_mask);

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

        last_child_pid = fork();
        if (last_child_pid == 0) {
            sigprocmask(SIG_SETMASK, &old_mask, NULL);

            execvp(argv[0], argv);
            perror("execvp");
            exit(EXIT_FAILURE);
        } else if (last_child_pid > 0) {
            waitpid(last_child_pid, NULL, 0);
            last_child_pid = 0;
        } else {
            perror("fork");
        }
    }

    return 0;
}
