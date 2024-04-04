#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_LENGTH 1024

int main() {
    char cmd[MAX_LENGTH];
    pid_t pid;
    int status;

    while (1) {
        printf("$ ");
        fflush(stdout);

        if (fgets(cmd, MAX_LENGTH, stdin) == NULL) {
            break; 
        }

        cmd[strcspn(cmd, "\n")] = 0;

        if (strcmp(cmd, "exit") == 0) {
            break;
        }

        pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            exit(1);
        } else if (pid == 0) {
            execlp(cmd, cmd, (char *)NULL);
            perror("Exec failed");
            exit(1);
        } else {
            wait(&status);
        }
    }

    return 0;
}
