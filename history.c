/**
 * @file
 *
 * Contains functions for creating the history list of the most recent 100
 * commands used.
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logger.h"
#include "history.h"

/**
 * Contains array of commands in history, the history limit, and # of total cmds
 */
struct history_struct {
    struct history_cmd *history;
    int lim;
    int cmd_count; 
    
};
/**
 * inviidual chistory command struct, containing the string and index
 */
struct history_cmd {
    int cmd_num;
    char *cmd;
};

/**The current index of the cmd in the circular array */
static int cmd_index = 0;

/**Static declartion of history struct*/
static struct history_struct *his_struct;

void hist_init(unsigned int limit)
{
    his_struct = malloc(sizeof(struct history_struct));
    //LOG("size of history struct: %ld\n", sizeof(struct history_struct));
    his_struct->history = calloc(limit, sizeof(struct history_cmd));

    his_struct->lim = limit;
    his_struct->cmd_count = 0;
}

void hist_destroy(void)
{
    int num_free = (cmd_index+1);
    if (his_struct->cmd_count > his_struct->lim) {
        num_free = his_struct->lim;
    }
    for(int i = 0; i < num_free; i++) {
        free(his_struct->history[i].cmd);
    }
    free(his_struct->history);
    free(his_struct);
}

void hist_add(const char *cmd)
{

    if(his_struct->cmd_count >= his_struct->lim) {
        cmd_index = his_struct->cmd_count % his_struct->lim;
        free(his_struct->history[cmd_index].cmd);
    } else {
        cmd_index = his_struct->cmd_count;
    }
    his_struct->history[cmd_index].cmd = strdup(cmd);
    //LOG("cmd[%d] = %s\n", cmd_index, his_struct->history[cmd_index].cmd);
    his_struct->cmd_count++;
    his_struct->history[cmd_index].cmd_num = his_struct->cmd_count;
    fflush(stdout);
}

void hist_print(void)
{
    int cmd_i = cmd_index;
    for(int i = 0; i < his_struct->lim; i++) {
        cmd_i++;
        if(cmd_i == his_struct->lim) {
            cmd_i = 0;
        }
        if(his_struct->history[cmd_i].cmd) {
            printf("%d  %s\n", his_struct->history[cmd_i].cmd_num,
                his_struct->history[cmd_i].cmd);
        }
    }
    fflush(stdout);
}

const char *hist_search_prefix(char *prefix)
{
    int cmd_i = cmd_index;

    for(int i = 0; i < his_struct->lim; i++) {
        if(cmd_i < 0) {
            if(his_struct->cmd_count <= his_struct->lim) {
                break;
            } else {
                cmd_i = (his_struct->lim - 1);
            }
        }
        if(strstr(his_struct->history[cmd_i].cmd, prefix) != NULL) {
            LOG("command:%s\n", his_struct->history[cmd_i].cmd);
            return(his_struct->history[cmd_i].cmd);
        }
        cmd_i--;
    }
    return NULL;
}

const char *hist_search_cnum(int command_number)
{
    if(command_number > his_struct->cmd_count) {
        return NULL;
    }

    LOG("command_count:%d\n", his_struct->cmd_count);
    if((command_number + 100) < his_struct->cmd_count+1) {
       return NULL; 
    }
    int cmd_i = (command_number % 100) - 1;
    if(cmd_i < 0) {
        cmd_i = 99;
    }
    if (his_struct->history[cmd_i].cmd != NULL) {
        LOGP("are we seg faulting here?\n");
        return (his_struct->history[cmd_i].cmd);
    }
    return NULL;
}

unsigned int hist_last_cnum(void)
{
    if(his_struct->history[cmd_index].cmd_num) {
        return his_struct->history[cmd_index].cmd_num;
    }
    return -1;
}
