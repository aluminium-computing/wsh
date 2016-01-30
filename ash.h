/* Copyright (c) 2013 Aim Inc. (under Aim Open-Source License) */


/***** GLOBAL VARIABLES *****/

#define MAX_COMMAND 4096
#define MAX_ARGS 2048
#define GC_MAX_ENV 2048

/* Data Types */
/* Boolean */
#define BOOL char
#define TRUE 1
#define FALSE 0



/* Functtions */
void show_prompt();
BOOL is_shell_builtin(const char *c_cmd);
int exec_builtin(int argc, char **argv, char **env);
int exec_command(int argc, char **argv, char **env);
int max(int a, int b);
void spec_char_expand(char *command_line);
BOOL find_exec(char *dir, char **argv);
int is_spec_char(char chars);
