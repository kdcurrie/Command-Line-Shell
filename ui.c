/**
 * @file
 *
 * Text-based UI functionality. These functions are primarily concerned with
 * interacting with the readline library.
 */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <locale.h>
#include <errno.h>
#include <sys/types.h>
#include <pwd.h>

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "shell.h"

/**struct containing info for prompt string*/
typedef struct {
    char face[20];
    int cmd_num;
    char username[32];
    char hostname[255];
    char cwd[4096];
    int exit_status;
    char userhome[30];
} prompt_config;

/** Prototype */
static int readline_init(void);

/** prompt string */
static char prompt_str[160];

/** prompt string struct */
static prompt_config *pconfig;

/**boolean indicator for if we are scripting */
static bool scripting = false;

/** line that will be used for reading input from user or script */
static char *line;

/** size of the buffer */
static size_t line_sz;

void free_prompt(void) {
    free(pconfig);
}

void set_status_face(int status) {
    if (status == 0) {
        strncpy(pconfig->face, "\033[0;32m:-)\033[0m", 20);
    } else {
        strncpy(pconfig->face, "\033[0;31m:-(\033[0m", 20);
    }
}

void get_cwd(void) {
    getcwd(pconfig->cwd, 4096);
    pconfig->cwd[4095] = '\0';
    if(strstr(pconfig->cwd, pconfig->userhome) == pconfig->cwd) {
        size_t end = strspn(pconfig->cwd, pconfig->userhome);
        size_t len = strlen(pconfig->cwd);
        if(strcmp(pconfig->userhome, pconfig->cwd) != 0) {
            end = end - 1;
        }
        len = len - end;
        //LOG("end: %zu\n", end);
        char homelesscwd[4096];
        strncpy(homelesscwd, (pconfig->cwd) + end, len);
        homelesscwd[len] = '\0';
        snprintf(pconfig->cwd, 4096, "~%.48s", homelesscwd);
    }

    //LOG("cwd: %s\n", pconfig->cwd);
}

void init_prompt_config(void) {
    pconfig->exit_status = get_exit_status();
    set_status_face(pconfig->exit_status);
    pconfig->cmd_num = 0;

    getlogin_r(pconfig->username, 31);
    pconfig->username[31] = '\0';

    gethostname(pconfig->hostname, 255);

    struct passwd *pw = getpwuid(getuid());
    strncpy(pconfig->userhome, pw->pw_dir, strlen(pw->pw_dir));

    //LOG("userhome: %s /n", pconfig->userhome);
    get_cwd();
}

void init_ui(void)
{
    pconfig = malloc(sizeof(prompt_config));
    memset(pconfig, 0, sizeof(prompt_config));

    init_prompt_config();

    //LOG("size of prompt struct: %ld\n", sizeof(prompt_config));
    LOGP("Initializing UI...\n");

    char *locale = setlocale(LC_ALL, "en_US.UTF-8");
    LOG("Setting locale: %s\n",
            (locale != NULL) ? locale : "could not set locale!");

    if (isatty(STDIN_FILENO)) {
        //LOGP("stdin is a TTY; entering interactive mode\n");
        scripting = false;
    } else {
        scripting = true; 
    }

    rl_startup_hook = readline_init;
}

char *prompt_line(void) {
    pconfig->exit_status = get_exit_status();
    set_status_face(pconfig->exit_status);

    get_cwd();

    snprintf(prompt_str, 160, "[%s]-[%d]-[%.32s@%.32s:%.48s]> ", pconfig->face, 
            pconfig->cmd_num++, pconfig->username, pconfig->hostname,
            pconfig->cwd);
    return prompt_str;
}

char *read_command(void)
{
    if (scripting == false) {
        //LOGP("stdin is a TTY; entering interactive mode\n");
        return readline(prompt_line());
    } else {
        line = NULL;
        line_sz = 0;
        //LOGP("data piped in on stdin; entering script mode\n");
        ssize_t read_sz = getline(&line, &line_sz, stdin);
        //LOG("Line: %s\n", line);
        if (read_sz == -1) {
            perror("no line\n");
            free(line);
            return NULL;
        }
        if (line[read_sz - 1] == '\n') {
            line[read_sz - 1] = '\0';
            //LOGP("Removing newline?\n");
        }
        //LOG("Line: %s\n", line); 
        return line;
    } 

}


int readline_init(void)
{
    rl_bind_keyseq("\\e[A", key_up);
    rl_bind_keyseq("\\e[B", key_down);
    rl_variable_bind("show-all-if-ambiguous", "on");
    rl_variable_bind("colored-completion-prefix", "on");
    rl_attempted_completion_function = command_completion;
    return 0;
}

int key_up(int count, int key)
{
    char *command = strdup(rl_line_buffer); //free this later

    LOG("reverse searching for: %s\n", command);
    /* Modify the command entry text: */
    rl_replace_line("User pressed 'up' key", 1);

    /* Move the cursor to the end of the line: */
    rl_point = rl_end;

    // TODO: reverse history search

    return 0;
}

int key_down(int count, int key)
{
    /* Modify the command entry text: */
    rl_replace_line("User pressed 'down' key", 1);

    /* Move the cursor to the end of the line: */
    rl_point = rl_end;

    // TODO: forward history search

    return 0;
}

char **command_completion(const char *text, int start, int end)
{
    /* Tell readline that if we don't find a suitable completion, it should fall
     * back on its built-in filename completion. */
    rl_attempted_completion_over = 0;

    return rl_completion_matches(text, command_generator);
}

/**
 * This function is called repeatedly by the readline library to build a list of
 * possible completions. It returns one match per function call. Once there are
 * no more completions available, it returns NULL.
 */
char *command_generator(const char *text, int state)
{
    // TODO: find potential matching completions for 'text.' If you need to
    // initialize any data structures, state will be set to '0' the first time
    // this function is called. You will likely need to maintain static/global
    // variables to track where you are in the search so that you don't start
    // over from the beginning.

    return NULL;
}
