/* Copyright (c) 2013 Aim Inc. (under Aim Open-Source License)

   This file manages history for ash (Aim SHell).
*/

enum HIST_CMD {
  HIST_INIT,
  HIST_ADD,
  HIST_PRINT,
  HIST_GET_PREV,
  HIST_GET_NEXT
};

void history_impl(enum HIST_CMD cmd, char *data);
char *history_get(enum HIST_CMD cmd);
