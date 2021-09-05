/* cmd.c - functions and structures used for reading and process kashell
       input commands. */

/* Copyright (C) Kenniston Arraes Bonfim.

   This file is part of KaShell.

   KaShell is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   KaShell is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   GNU General Public License - <http://www.gnu.org/licenses/>.
*/
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <stdbool.h>
#include <sys/types.h>
#include "cmd.h"

#if !defined(__linux__)
#ifndef _ULONG
typedef unsigned long ulong;
#define _ULONG
#endif
#endif

#define OUT_SUBCMD ">"
#define IN_SUBCMD "<"
#define QUIT_CMD "quit"

struct termios orig_termios;    // Original termios configuration.

/* Remove all white space from the beginning and end of the string.. */
void str_trim(char * s) {
    char *start = s;
    ulong length = strlen(start);

    while (isspace(start[length - 1])) {
        start[--length] = 0;
    }
    while (*start && isspace(*start)) {
        ++start, --length;
    }
    memmove(s, start, length + 1);
}

/* This function parses the command line parameters
   and updates the corresponding field in the given
   task. */
void parse_cmd_params(task_t *task, char *params) {
    char *paramstr = calloc(1, strlen(params) + 1);
    strcpy(paramstr, params);

    // check for output character in params
    char *out = strstr(paramstr, OUT_SUBCMD);
    long outidx = -1;
    if (out != NULL) {
        outidx = out - paramstr;
        strncpy(task->outfile, out + sizeof(OUT_SUBCMD), sizeof(task->outfile));
        str_trim(task->outfile);
    }

    // check for input character in params
    char *in = strstr(paramstr, IN_SUBCMD);
    long inidx = -1;
    if (out != NULL) {
        inidx = in - paramstr;
        strncpy(task->infile, out + sizeof(IN_SUBCMD), sizeof(task->infile));
        str_trim(task->infile);
    }

    // get command from params
    ulong length = out ? strlen(out) : 0;
    char *cmd = calloc(1, strlen(paramstr) - length + 1);
    strncpy(cmd, paramstr, strlen(paramstr) - length);
    str_trim(cmd);

    // extract command parameters
    char *param;
    int count = 0;
    while ((param = strsep(&cmd, " "))) {
        if (strcmp(task->cmd, "") == 0) {
            strcpy(task->cmd, param);
            continue;
        }
        task->params[count++] = param;
    }
}

/* This function create a new command pipeline.
   In the command line, Tasks are separated by a pipe.
   A Task has the program to run, its optional parameters
   and an optional output file name. */
task_t* create_pipeline(char *str) {
    task_t *head = NULL;
    task_t *cur = NULL;
    char *cmd = NULL;
    char *cmdstr = calloc(1, strlen(str) + 1);

    strcpy(cmdstr, str);
    while ((cmd = strsep(&cmdstr, "|"))) {
        task_t *t = calloc(1, sizeof(task_t));
        if (t != NULL) {
            head = head ?: t;
            if (cur != NULL) {
                cur->next = t;
            }
            str_trim(cmd);
            parse_cmd_params(t, cmd);
            cur = t;
        } else {
            perror("Error allocating memory to the command.");
            return NULL;
        }
    }
    return head;
}

/* This function prints the pipeline data to stdout. */
void print_pipeline(task_t *tasks) {
    task_t *tmp;
    task_t *cur = tasks;

    printf("Pipeline:\n");
    while (cur) {
        tmp = cur->next;
        printf("\t=> %s", cur->cmd);
        size_t n = sizeof cur->params / sizeof *cur->params;

        char *p = NULL;
        int count = 0;
        while ((p = cur->params[count++]) != NULL) {
            printf(" %s", p);
        }

        if (strcmp(cur->outfile, "") != 0) {
            printf(" > %s", cur->outfile);
        }
        printf("\n");
        cur = tmp;
    }
}

/* This function remove the allocated memory from a pipeline. */
void free_pipeline(task_t *tasks) {
    task_t *tmp;
    task_t *cur = tasks;
    while (cur) {
        tmp = cur->next;
        free(cur);
        cur = tmp;
    }
}

/* Disable input raw mode in keyboard */
void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

/* Enable input raw mode in keyboard */
void enableRawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHOCTL);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

/* This provides a Shell command line loop. All user supplied
   data are trimmed (to remove white spaces) before being
   processed by the command line parser function. */
void run_shell() {
    //enableRawMode();
    do {
        printf("cmd> ");

        char *cmd = 0;
        size_t cmdlen = 0;
        getline(&cmd, &cmdlen, stdin);
        str_trim(cmd);

        if (strcasecmp(cmd, QUIT_CMD) == 0) {
            break;
        }

        task_t *tasks = create_pipeline(cmd);
        if (tasks == NULL) continue;
        print_pipeline(tasks);

        free_pipeline(tasks);
        free(cmd);
    } while (true);

    printf("\nGood bye!\n\n");
}
