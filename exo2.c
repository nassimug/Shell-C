#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

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
    char test_str[] = "commande -v toto tata";
    char *argv[20];
    int argc = parse_line(test_str, argv);

    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    if (argv[argc] == NULL) {
        printf("argv[%d] = NULL\n", argc);
    }

    return 0;
}


