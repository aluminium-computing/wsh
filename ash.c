/* Copyright (c) 2013, Aim Inc. (under Aim Open-Source License)

   This is (so far) the main (pun intended) file in ash (Aim SHell)
*/
#include "ash.h"
#include "history.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <dirent.h>
#include <unistd.h>
#include <ncurses.h>

int main(int argc, char **argv, char **env) {
  char *command_line, *copy_buf;
  char *command_line_zero;
  int cmd_argc = 0;
  char *cmd_argv[MAX_ARGS];

  history_impl(HIST_INIT, "");

  initscr();
  raw();
  keypad(stdscr, TRUE);
  noecho();
  scrollok(stdscr, TRUE);

  /* Initialize shell... */
  command_line = malloc(MAX_COMMAND * sizeof(char));
  copy_buf = malloc(MAX_COMMAND * sizeof(char));
  command_line_zero = command_line;
  printw("Welcome to W I L D E B E E S T!\n");
  printw("You are running wsh (Wildebeest SHell) v0.1\n");
  printw("Copyright © 2013, 2016 Aluminium Computing, Inc\n");

  while (1) {
    refresh();
    show_prompt();
    memset(command_line, 0, MAX_COMMAND);
    /* OLD: fgets(command_line, MAX_COMMAND, stdin); */
    int i = 0, cursor = 0 ;
    int ch, row, col;
    while ((ch = getch()) != '\n') {
      getyx(stdscr, row, col);
      switch (ch) {
        case 3:
          memset(command_line, 0, MAX_COMMAND);
          printw("\nLicense is Aim Public License: you any do anything you want\n");
          printw("as long as you acknowledge that Aluminium Computing, Inc. is its creator.\n\n");
          printw("Copyright © 2013, 2016 Aluminium Computing, Inc.\n\n");
          show_prompt();
          i = cursor = 0;
          break;
        case 4:
          endwin();
          exit (1);
        case KEY_UP:
          command_line = history_get(HIST_GET_PREV);
          move(row, col - i);
          for ( ; i>0; i--) delch();
          printw("%s", command_line);
          i = strlen(command_line);
          refresh();
          break;
        case KEY_DOWN:
          command_line = history_get(HIST_GET_NEXT);
          move(row, col - i);
          for ( ; i>0; i--) delch();
          printw("%s", command_line);
          i = strlen(command_line);
          if (0 == i) command_line = command_line_zero;
          refresh();
          break;
        case KEY_LEFT:
          if (cursor == 0) break;
          move(row, --col);
          refresh(); 
          cursor--;
          break;
        case KEY_RIGHT:
          if (cursor == i) break;
          move(row, ++col);
          refresh();
          cursor++;
          break;
        case KEY_BACKSPACE: {
          move(row, col-1);
          delch();
          refresh();
          i--;
          cursor--;
          break;
        }
        default:
          printw("%c", ch);
          if (cursor != i) 
            strncpy(copy_buf, &command_line[cursor], MAX_COMMAND);
          command_line[cursor] = ch;
          cursor++; i++;
          if (cursor != i) {
            strncpy(&command_line[cursor], copy_buf, MAX_COMMAND);
            addstr(copy_buf);
            getyx(stdscr, row, col);
            move(row, col - (i - cursor));
          }
      }
    }

    printw("\n");
    command_line[i] = '\0';
    /* Check |command_line| for special characters and replace them. */
    spec_char_expand(command_line);
    history_impl(HIST_ADD, command_line);
    cmd_argc=0;
    cmd_argv[cmd_argc]=strtok(command_line, " \n");
    while (cmd_argv[cmd_argc] != NULL) {
      cmd_argc++;
      cmd_argv[cmd_argc]=strtok(NULL, " \n");
    }
    if (cmd_argc > 0) {
      if (is_shell_builtin(cmd_argv[0])) {
        exec_builtin(cmd_argc, cmd_argv, env);
      }
      else {
        exec_command(cmd_argc, cmd_argv, env);
      }
    }
  }
}


/*** FUNCTIONS ***/



void show_prompt() {
  char *prompt;
  prompt = getenv("wsh_prompt");
  if (prompt == NULL) {
    printw("Wildebeest%%");
  } else {
    printw("%s", prompt);
  }
  
}

BOOL is_shell_builtin(const char *c_cmd) {
  int command_length=strlen(c_cmd);
  if (strncmp(c_cmd, "exit", max(command_length,4)) ==0) return TRUE;
  
  if ((strncmp(c_cmd, "chow",      max(command_length,4)) ==0) ||
      (strncmp(c_cmd, "show_vars", max(command_length,9)) ==0)) return TRUE;
  
  if (strncmp(c_cmd, "history", max(command_length,7)) ==0) return TRUE;
  
  if ((strncmp(c_cmd, "cd",    max(command_length, 2)) == 0) ||
      (strncmp(c_cmd, "chdir", max(command_length, 5)) == 0)) return TRUE;
  
  if (strncmp(c_cmd, "version", max(command_length, 7)) == 0) return TRUE;
  return FALSE;
}

int exec_builtin(int argc, char **argv, char **env) {
  int command_length=strlen(argv[0]);
  if (strncmp(argv[0], "exit", max(command_length,4)) ==0) {
    endwin();
    exit(0);
  }
  if ((strncmp(argv[0], "chow", max(command_length,4)) ==0) ||
      (strncmp(argv[0], "show_vars",max(command_length, 9)) ==0)) {
    int i=0;
    while ((env[i] != 0) && (i<=GC_MAX_ENV)) {
      printf("%s\n", env[i]);
      i++;
    }
  }

  if (strncmp(argv[0], "history", max(command_length,7)) ==0) {
    history_impl(HIST_PRINT, "");
  }

  if ((strncmp(argv[0], "cd", max(command_length, 2)) ==0) ||
     (strncmp(argv[0], "chdir", max(command_length, 5)) ==0)) {
    if (argv[1] != NULL) chdir(argv[1]);
    if (argv[1] == NULL) printw("wsh: Error 01 encountered: not enough arguments\n");
  }
  if (strncmp(argv[0], "version", max(command_length, 7)) == 0) {
    printw("You are running wsh (Wildebeest SHell) v0.1\n");
    printw("Copyright © 2013, 2016 Aluminium Computing, Inc\n"); }
  return -1;
}

int exec_command(int argc, char **argv, char **env) {
  char path[MAX_COMMAND];
  char *path_token;
  int i=0;
  while ((env[i] !=0) && (i<=GC_MAX_ENV)) {
   if (strncmp(env[i], "PATH=", 5) ==0 ||
       strncmp(env[i], "path=", 5) ==0) {
     strncpy(path, &env[i][5], MAX_COMMAND);
     break;
   }
   i++; 
  } 
  /* tokenize path */
  path_token = strtok(path, ":");
  while (path_token != NULL) {
    
    if (find_exec(path_token, argv)== TRUE) return 1;
    path_token = strtok(NULL, ":");
   
  }
  printf("\nwsh: %s: command not found.\n", argv[0]); 
  return 0;
}
  

int  max(int a, int b) {
  if (b > a) return b;
  return a;
}

BOOL find_exec (char *dir, char **argv) {
  DIR *dirp;
  struct dirent *content;
  dirp=opendir(dir);
  if (dirp==NULL) return FALSE;
  while  ( (content=readdir(dirp)) ){
   if (strncmp(content->d_name, argv[0], 256)==0){
     char full_path[MAX_COMMAND];
     strncpy(full_path, dir, MAX_COMMAND);
     strncat(full_path, "/", MAX_COMMAND);
     strncat(full_path, argv[0], MAX_COMMAND);
     int i =1;
     while (argv[i] !=NULL) {
       strncat(full_path, " ", MAX_COMMAND);
       strncat(full_path, argv[i], MAX_COMMAND);
       i++;
     }
     /* printf("DEBUG: full_path=%s\n", full_path); */
     FILE *fp;
     char line[MAX_COMMAND];
     fp = popen(full_path, "r");
     while (fgets(line, sizeof line, fp) != NULL) {
       printw(line);
       refresh();
     }
     
     pclose(fp);
     
     /* def_prog_mode();
     endwin();
     system(full_path);
     reset_prog_mode();
     refresh();
     */
     closedir(dirp);
     return TRUE;
   }
  }
  closedir(dirp);
  return FALSE;
}


void spec_char_expand(char *command_line) {
  int i=0;
  while (command_line[i] != '\0') {
    int spc;
    if ( (spc = is_spec_char(command_line[i])) ) {
      if (spc == 589) {
        char temp_command_line[MAX_COMMAND];
        char *home_dir;
        int len;
        home_dir=getenv("HOME");
        len = strlen(home_dir);
        strncpy(temp_command_line, command_line, MAX_COMMAND);
        strncpy(&command_line[0], &temp_command_line[0], i);
        strncpy(&command_line[i], home_dir, len);
        strncpy(&command_line[i+len], &temp_command_line[i+1], MAX_COMMAND);
      }
      else if (spc == 612) {
        command_line[i] = '\0';
        /* After a #, we can break out of the while loop. Hash means 'stop 
           processing characters after this'.
        */
        break;
      }
    }
    i++;
  } 
}



int is_spec_char(char chars) {
  int i=0;
  if (chars == '~') return 589;
  if (chars == '#') return 612;
  return FALSE;
}
