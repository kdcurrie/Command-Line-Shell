/**
 * @file
 *
 * Text-based UI functionality. These functions are primarily concerned with
 * interacting with the readline library.
 */

#ifndef _UI_H_
#define _UI_H_

/**
 * autocompletes command for user
 */
char **command_completion(const char *text, int start, int end);

/**
 * generates the command for autocompletion
 */
char *command_generator(const char *text, int state);

/**
 * Initializes the UI
 */
void init_ui(void);

/**
 * Used to grab last history command
 */
int key_up(int count, int key);

/**
 * Used to grab next history command
 */
int key_down(int count, int key);

/**
 * Creates the prompt line string
 */
char *prompt_line(void);

/**
 * Reads the command either from user input or from a script
 */
char *read_command(void);

/**
 * frees memory allocated for prompt struct
 */
void free_prompt(void);

/**
 * gets the face indicating succesful status code or not successful status code
 */
void set_status_face(int status);

/**
 * sets the current working directory
 */
void get_cwd(void);

/**
 * initalizes the different data members needed for the prompt string
 */
void init_prompt_config(void);

#endif
