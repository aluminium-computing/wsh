/* Copyright (c) 2013 Aim Inc. (under Aim Open-Source License)

   This file manages history for ash (Aim SHell).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "history.h"

#define FALSE 0
#define TRUE 1
#define MAX_COMMAND  4096

struct history_entry *history_list;
struct history_entry *history_list_begin;
struct history_entry *prev_cmd;
int HIST_INIT_COMPLETE = FALSE;

struct history_entry {
  int index;
  char command_line[MAX_COMMAND];
  struct history_entry *prev;
  struct history_entry *next;
};

int bang(char *data);

void history_impl(enum HIST_CMD cmd, char *data) {

  switch (cmd) {
    case HIST_INIT:
      if (HIST_INIT_COMPLETE) break;
      history_list = (struct history_entry*)malloc(sizeof(struct history_entry));
      history_list->index = 0;
      history_list->prev = 0;
      history_list->next = 0;
      history_list_begin = history_list;
      prev_cmd = history_list;
      HIST_INIT_COMPLETE = TRUE;
      break;
    case HIST_ADD: {
      if (history_list->prev && bang(data)) break;
      
      struct history_entry *new_command;
      new_command = malloc(sizeof(struct history_entry));
      new_command->next = 0;
      new_command->index = 1 + history_list->index;
      strncpy(new_command->command_line, data, MAX_COMMAND);
      new_command->prev = history_list;
      history_list->next = new_command;
      history_list = new_command;
      prev_cmd = history_list;
      break;
    }
    case HIST_PRINT: {
      struct history_entry *list;
      list = history_list_begin;
      while (list->next) {
        list = list->next;
        printw("%3d %s\n", list->index, list->command_line);
      }
      break;
    }
    default:
      break;
  }
  return;
}

char *history_get(enum HIST_CMD cmd) {
  char *return_cmd = "";
  switch (cmd) {
    case HIST_GET_PREV: {
      if (prev_cmd->command_line) {
        return_cmd = prev_cmd->command_line;
      }
      if (prev_cmd->prev) prev_cmd = prev_cmd->prev;
      break;
    }
    case HIST_GET_NEXT: {
      if (prev_cmd->next) prev_cmd = prev_cmd->next;
      if (prev_cmd->next) {
        return_cmd = prev_cmd->next->command_line;
      }
      break;
    }
    default:
      break;
  }
  return return_cmd;
}



int bang(char *data) {
  if (data[0] != '!') return FALSE;
  if (data[1] == '!') {
    strncpy(data, history_list->command_line, MAX_COMMAND);
    return TRUE;
  } 
  
  if ('0' <= data[1] && data[1] <= '9') {
    /* TODO: implemtent number-handling */
    return FALSE;
  }
  char *cmd_frag = &data[1];
  struct history_entry *list_match = history_list;
  while (list_match->command_line) {
    if (strncmp(list_match->command_line, cmd_frag, (strlen(cmd_frag)-1)) == 0) {
      strncpy(data, list_match->command_line, MAX_COMMAND);
      return TRUE;
    }
    if (list_match->prev) {
      list_match = list_match->prev;
    } else {
      return FALSE;
    } 
  }
  return 2;
}

 
