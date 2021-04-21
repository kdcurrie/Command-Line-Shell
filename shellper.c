/**
 * @file
 *
 * Contains useful helper functions for executing commands and pipes, as well as
 * misc. helper functions for our shell driver.
 */

#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <locale.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "logger.h"
#include "shellper.h"

char* get_home_directory() {
    char *homedir;

    if ((homedir = getenv("HOME")) == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }

    return homedir;
}
bool check_pipe(char *command) {
    if (strstr(command, "|") != NULL) {
        return true;
    }
    return false;
}

int execute(char **args, int line_sz) {
    struct command_line *cmds = malloc(sizeof(struct command_line) * line_sz);
    setup_cmds(cmds, line_sz, args);
    int ret = execute_pipeline(cmds);
    free(cmds);

    return ret;
}

int setup_cmds(struct command_line *cmds, int num_tokens, char **args) {

    
    int cmd_count = 0;
    cmds[cmd_count].tokens = &args[cmd_count];
    cmds[cmd_count].stdout_pipe = true;
    cmds[cmd_count].stdout_file = NULL;
    cmd_count++;

    for(int j = 0; j <= num_tokens; j++) {
        if (j == num_tokens) {
            args[j] = (char *) NULL;
        } else if (strcmp(args[j], "<") == 0) {//new
            args[j] = (char *) NULL;
            cmds[cmd_count-1].stdin_file = args[j+1];
        } else if (args[j][0] == '>') {
            if (args[j][1] == '>') {
                cmds[cmd_count-1].append = true;
            }
            args[j] = (char *) NULL;
            cmds[cmd_count-1].stdout_file = args[j+1];
        } else if (strcmp(args[j], "|") == 0) {
            args[j] = (char *) NULL;
            cmds[cmd_count].tokens = &args[j+1];
            cmds[cmd_count].stdout_pipe = true;
            cmds[cmd_count].stdout_file = NULL;
            cmd_count++;
        }
    } 
    cmds[cmd_count-1].stdout_pipe = false;
    
    /* debugging snippet
    for (int k = 0; k < cmd_count; k++) {
        int l = 0;
        while(cmds[k].tokens[l]) {
            LOG("cmd[%d]->tokens[%d] = %s\n", k, l, cmds[k].tokens[l]);
            l++;
        }
        LOG("cmd[%d]->tokens[%d] = %s\n", k, l, cmds[k].tokens[l]);
    }
    */
    return cmd_count;
}

int execute_pipeline(struct command_line *cmds)
{
    if (cmds->stdout_pipe == false) {
        int out_fd;
        int saved_out = dup(STDOUT_FILENO);
        if (cmds->stdout_file != NULL) {
            if(cmds->append == false) {
                out_fd = open(cmds->stdout_file, O_RDWR | O_CREAT | O_TRUNC, 0666);
                LOG("Not append?: %d\n", cmds->append);
            } else {
                out_fd = open(cmds->stdout_file, O_APPEND | O_RDWR, 0666);
                LOG("Are we making it to append?: %d\n", cmds->append);
            }
            dup2(out_fd, STDOUT_FILENO);
        }
        int in_fd;
        if (cmds->stdin_file != NULL) {
            in_fd = open(cmds->stdin_file, O_RDONLY, 0666);
            dup2(in_fd, STDIN_FILENO);
        }
        int ret = execvp(cmds->tokens[0], cmds->tokens);
        if (ret == -1) {
            dup2(saved_out, STDOUT_FILENO);
            printf("Invalid command, you dummy!\n");
        }
        close(saved_out);
        close(out_fd);
        close(in_fd);
        close(STDOUT_FILENO);
        close(STDIN_FILENO);
        return 0;
    }
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return -1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }
    else if (pid == 0) {
        /* Child */
        dup2(fd[1], fileno(stdout));
        close(fd[0]);
        execvp(cmds->tokens[0], cmds->tokens);
        close(fd[1]);
    } else {
        /* Parent */
        dup2(fd[0], fileno(stdin));
        close(fd[1]);
        execute_pipeline(cmds + 1);
        close(fd[0]);
    }
    return 0;
}

/**
 * Tokenizer Function
 */ 
char *next_token(char **str_ptr, const char *delim)
{
    if (*str_ptr == NULL) {
        return NULL;
    }

    size_t tok_start = strspn(*str_ptr, delim);
    size_t tok_end = strcspn(*str_ptr + tok_start, delim);

    if (tok_end == 0) {
        *str_ptr = NULL;
        return NULL;
    }
    char *current_ptr = *str_ptr + tok_start;

    *str_ptr += tok_start + tok_end;

    if (**str_ptr == '\0') {
        *str_ptr = NULL;
    } else {
        **str_ptr = '\0';
        (*str_ptr)++;
    }

    return current_ptr;
}

