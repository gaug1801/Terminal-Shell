//
//  A2_462.c
//  
//
//  Created by Gabriel Augustin on 6/29/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>

#define CMD_LENGTH 1024
#define TOKEN_LENGTH 64
#define DELIM " \t\r\n\a"

char* built_in_cmds[] = {"cd", "exit", "path"};
char* path = "/bin/";

void shell_loop(void);
char* shell_read_line(void);
char** shell_split_line(char*);
int shell_execute(char**);
int shell_cd(char**);
int shell_exit(char**);
int shell_path(char**);

int (*built_in_funcs[])(char **) = {&shell_cd, &shell_exit, &shell_path};

int main(int argc, char **argv) {
    shell_loop();
    return EXIT_SUCCESS;
}

void shell_loop(void) {
    char *line;
    char **args;
    int status;

    do {
        printf("wish> ");
        line = shell_read_line();
        args = shell_split_line(line);
        status = shell_execute(args);
        
        free(line);
        free(args);
    } while (status);
}

char* shell_read_line(void) {
    char* line = NULL;
    size_t bufsize = 0;
    getline(&line, &bufsize, stdin);
    return line;
}

char** shell_split_line(char* line) {
    int bufsize = TOKEN_LENGTH, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    token = strtok(line, DELIM);
    while (token != NULL) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += TOKEN_LENGTH;
            tokens = realloc(tokens, bufsize * sizeof(char*));
        }

        token = strtok(NULL, DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

int shell_execute(char** args) {
    int i;
    if (args[0] == NULL) {
        return 1;
    }

    for (i = 0; i < 3; i++) {
        if (strcmp(args[0], built_in_cmds[i]) == 0) {
            return (*built_in_funcs[i])(args);
        }
    }

    pid_t pid, wpid;
    int status;
    pid = fork();

    if (pid == 0) {
        if (execv(strcat(path, args[0]), args) == -1) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
    } else {
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}

int shell_cd(char **args) {
    if (args[1] == NULL) {
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
    } else {
        if (chdir(args[1]) != 0) {
            char error_message[30] = "An error has occurred\n";
            write(STDERR_FILENO, error_message, strlen(error_message));
        }
        
    }
    return 1;
}

int shell_exit(char **args) {
    return 0;
}

int shell_path(char **args) {
    if (args[1] == NULL) {
        char error_message[30] = "An error has occurred\n";
        write(STDERR_FILENO, error_message, strlen(error_message));
    } else {
        path = args[1];
    }
    return 1;
}
