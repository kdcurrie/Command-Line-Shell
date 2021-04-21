/**
 * @file
 *
 * Contains shell history data structures and retrieval functions.
 */

#ifndef _HISTORY_H_
#define _HISTORY_H_

/**
 * Initializes the history struct circular array
 */
void hist_init(unsigned int);

/**
 * Cleans up and frees the history circular array
 */
void hist_destroy(void);

/**
 * adds a command to the history circular array
 */
void hist_add(const char *);

/**
 * Prints all of the most recent commands
 */
void hist_print(void);

/**
 * Used to search the history with a string prefix
 */
const char *hist_search_prefix(char *);

/**
 * Used to search the history with a command number
 */
const char *hist_search_cnum(int);

/**
 * Returns the command number of the most recent command
 */
unsigned int hist_last_cnum(void);

#endif
