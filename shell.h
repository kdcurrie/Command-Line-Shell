/**
 * @file
 *
 * Header file containing built in functions for the shell, as well as other
 * helper functions for parsing each command.
 */

#ifndef _SHELL_H_
#define _SHELL_H_

/**
 * ingal handling function
 */
void sigint_handler(int signo);

/**
 * Returns most recent exit status
 */
int get_exit_status(void);

/**
 * Performs clean up of shell and exits
 */
int shutdown_shell(void);

/**
 * Prints shutdown message and shutdowns shell
 */
int exit_shell(int argc, char *args[]);

/**
 * Prints last 100 commands
 */
int print_history(int argc, char *args[]);

/**
 * Changes the current working directory
 */
int change_dir(int argc, char *args[]);

/**
 * Tokenizes the command
 */
int tokenize_command(char *command, char *args[]);

/**
 * Handles built in commands
 */
int handle_builtins(int argc, char *args[]);

#endif
