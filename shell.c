/**
 * @file
 *
 * Driver for shell program.
 */

#include <fcntl.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "shell.h"
#include "shellper.h"

static int status;
static char* command = NULL;
static int tokens = 0;

/**
 * Contains name of built in command and function pointer
 */
struct builtin_def {
    char name[128];
    int (*function)(int argc, char *args[]);
};

/**
 * Built in command array
 */
struct builtin_def builtins[] = {
    { "exit", exit_shell },
    { "bye", exit_shell },
    { "cd", change_dir },
    { "history", print_history },
    //{ "jobs", job_list },
};

/**
 * Runs sheesh shell
 */
int main(void) {

    init_ui();
    hist_init(100);

    signal(SIGINT, sigint_handler);

    while (true) {
        if(command != NULL) {
            free(command);
        }
        command = read_command();
        if (command == NULL) { 
            break;
        }
        //LOG("\nWe got a command line: %s\n", command);
        if(strcmp(command, "history") != 0 && command[0] != '!') {
            hist_add(command);
        }
        char *args[4096];

        if(tokenize_command(command, args) == 0) {
            continue;
        }

        status = handle_builtins(tokens, args);
        if (status == 0) {
            //LOG("making sure not here:%s \n", args[0]);
            continue;
        }
        //LOG("command:%s\n", command);
 
        pid_t child = fork();
        if (child == -1) {
            perror("fork");
        } else if (child == 0) { //we are the child
            int ret = execute(args, tokens);
            if (ret == -1) {
                perror("execute");
                printf("Invalid command, you dummy\n");
            }
            fflush(stdout);
            close(fileno(stdin));
            //if(status != 0) {
                printf("Invalid command, you dummy\n");
            //}
            //LOGP("logging here\n");
            //LOGP("free worked\n");
            exit(EXIT_FAILURE);//without this child will continue and become shell
        } else { 
            waitpid(child, &status, 0);
            //LOG("child exited. status: %d\n", status);
        }
    }
    shutdown_shell();
    return 0;
}

void sigint_handler(int signo) {
    if (isatty(STDIN_FILENO)) {
        printf("\n");
    }
    fflush(stdout);
}

int get_exit_status(void) {
    return status;
}

int shutdown_shell(void) {
    close(fileno(stdin));//don't need these
    close(fileno(stdout));
    close(fileno(stderr));
    hist_destroy();
    free(command);

    free_prompt();
    exit(EXIT_SUCCESS);
    return 1;
}

int exit_shell(int argc, char *args[]) {
    printf("Is this really the end? I don't want to di..\n");
    shutdown_shell();
    return 1;
}

int print_history(int argc, char *args[]) {
    hist_add("history");
    hist_print();
    return 1;
}

int change_dir(int argc, char *args[]) {
    int ret;
    ret = (args[1] == NULL) ? chdir(get_home_directory()) : chdir(args[1]);

    if (ret == -1) {
        perror("chdir");
    }
    return ret;
} 

int tokenize_command(char *command, char *args[]) {

    tokens = 0;
    char *next_tok = command;
    char *curr_tok;

    while ((curr_tok = next_token(&next_tok, " \t\n\r")) != NULL) {
        args[tokens++] = curr_tok;
    }
    //LOG("Made it here, checking if pipe & built ins:%s, & num_tokens: %d\n",command,              num_tokens);
    args[tokens] = NULL;
    if (args[0] == NULL) {
    //LOG("making sure not here:%d \n", tokens);
        return 0;
    }
    return 1;
}

int handle_builtins(int argc, char *args[]) {
    for(int i = 0; i < sizeof(builtins) / sizeof(struct builtin_def); ++i) {
        if (strcmp(args[0], builtins[i].name) == 0) {
            return builtins[i].function(argc, args);
        } else if (args[0][0] == '!') {
            char *bang;
            int cmd_len = strlen(command);
            char str[cmd_len+1];
            bang = str;
            strncpy(bang, command, cmd_len+1);
            if (args[0][1] == '!') {
                //LOG("last cnum:%d \n", hist_last_cnum());
                //LOG("last command:%s \n", hist_search_cnum(hist_last_cnum()));
                strncpy(command, hist_search_cnum(hist_last_cnum()), 
                        strlen(hist_search_cnum(hist_last_cnum())));
            } else if(atoi(args[0]+1)) {
                //LOG("args+1 converted:%d \n", atoi(args[0]+1));
                if(hist_search_cnum(atoi(args[0]+1)) != NULL) {
                    strncpy(command, hist_search_cnum(atoi(args[0]+1)), 
                            strlen(hist_search_cnum(atoi(args[0]+1))));
                }
            } else {
                if(hist_search_prefix(args[0]+1) != NULL) {
                    strncpy(command, hist_search_prefix(args[0]+1), 
                            strlen(hist_search_prefix(args[0]+1)));
                } 
            }
            if(tokenize_command(command, args) == 0) {
                return 0;
            }
            hist_add(bang);
            return 1; 
        }
    }
    for(int j = 0; j < argc; j++) {
        if (strstr(args[j], "#") != NULL) {
            LOG("comment arg full string: %s\n", args[j]);
            args[j] = (char *) NULL;
            tokens = j;
            return 1;
        }
    }
    return 1;
}
