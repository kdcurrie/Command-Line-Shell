/**
 * @file
 *
 * Header file containing useful helper functions for executing commands and 
 * pipes, as well as misc. helper functions for our shell driver.
 */

#include <stdbool.h>

#ifndef _SHELLPER_H_
#define _SHELLPER_H_

/**command line struct containing information need for executing commands*/
struct command_line {
    char **tokens;
    bool stdout_pipe;
    char *stdout_file;
    char *stdin_file;
    bool append;
};

/**
 * Returns the systems home directory 
 */
char* get_home_directory();

/**
 * Checks if the command is a pipe
 */
bool check_pipe(char *command);

/**
 * Begins the process of executing the command or pipe
 */
int execute(char **args, int line_sz);

/**
 * Setups up the command or pipe for execution 
 */
int setup_cmds(struct command_line *cmds, int num_tokens, char **args);

/**
 * Executes the pipe or single command
 */
int execute_pipeline(struct command_line *cmds);

/**
 * Retrieves the next token in the command string
 */
char *next_token(char **str_ptr, const char *delim);

#endif
