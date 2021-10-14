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
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <stdbool.h>
#include <unistd.h>
#include <pwd.h>
#include <linux/limits.h>
#include "cmd.h"
#include "jobctl.h"
#include "shutil.h"

#if !defined(__linux__)
#ifndef _ULONG
typedef unsigned long ulong;
#define _ULONG
#endif
#endif

#define OUT_SUBCMD ">"
#define IN_SUBCMD "<"
#define QUIT_CMD "quit"
#define DEFAULT_USER "user"

struct termios orig_termios;    // Original termios configuration.

/* This function validate the tasks in the pipeline. */
bool validade_tasks(task_t *tasks) {
    char msg[128];
    task_t *next;
    task_t *cur = tasks;
    int count = 1;
    while (cur) {
        next = cur->next;
        // Check the parameters in early tasks. Output file must be on last task.
        if (strcmp(cur->outfile, "") != 0 && next != NULL) {
            sprintf(msg, "Output file in task %d. The output to file must be on last task on pipeline", count);
            errno = EINVAL;
            perror(msg);
            return false;
        }

        // Check input parameters. Input with files cannot have other parameters.
        /*if (strcmp(cur->infile, "") != 0 && cur->params[1] != NULL) {
            sprintf(msg, "Too many parameter in task %d. Task with input file cannot have other parameters", count);
            errno = EINVAL;
            perror(msg);
            return false;
        }*/

        cur = next;
        count++;
    }

    return true;
}

/* This function parses the command line parameters
   and updates the corresponding field in the given
   task. */
void parse_cmd_params(task_t *task, char *params) {
    char *paramstr = calloc(1, strlen(params) + 1);
    strcpy(paramstr, params);
    char *original = paramstr;

    // check for output character in params.
    char *out = strstr(paramstr, OUT_SUBCMD);
    if (out != NULL) {
        strncpy(task->outfile, out + sizeof(OUT_SUBCMD), sizeof(task->outfile));
        str_trim(task->outfile);
        *out = 0;
    }

    // check for input character in params.
    char *in = strstr(paramstr, IN_SUBCMD);
    if (in != NULL) {
        strncpy(task->infile, in + sizeof(IN_SUBCMD), sizeof(task->infile));
        str_trim(task->infile);
        *in = 0;
    }

    // extract command parameters.
    char *param;
    int count = 0;
    str_trim(paramstr);
    while ((param = strsep(&paramstr, " "))) {
        if (strcmp(task->cmd, "") == 0) {
            strcpy(task->cmd, param);
        }
        task->params[count] = calloc(1, strlen(param) + 1);
        strcpy(task->params[count], param);
        count++;
    }
    free(original);
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
    char *original = cmdstr;

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
    free(original);

    return validade_tasks(head) ? head : NULL;
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
        int param_idx = 0;
        char *p;
        while ((p = cur->params[param_idx++]) != NULL) {
            free(p);
        }
        free(cur);
        cur = tmp;
    }
}

/* This provides a Shell command line loop. All user supplied
   data are trimmed (to remove white spaces) before being
   processed by the command line parser function. */
void run_shell() {
    char hostname[255];
    char username[255];

    gethostname(hostname, sizeof(hostname));
    struct passwd *p = getpwuid(getuid());
    if (p == NULL) {
        strcpy(username, DEFAULT_USER);
    }
    strcpy(username, p->pw_name);

    char cwd[PATH_MAX];
    do {
        getcwd(cwd, sizeof(cwd));
        char *path = laststr(cwd, '/');
        printf("[%s@%s] %s > ", username, hostname, path);

        char *cmd = 0;
        size_t cmdlen = 0;
        do {
            getline(&cmd, &cmdlen, stdin);
            if (cmd == NULL) continue;

            str_trim(cmd);
            if (strlen(cmd) > 0){
                break;
            }
        } while (true);

        if (strcasecmp(cmd, QUIT_CMD) == 0) {
            free(cmd);
            break;
        }

        task_t *tasks = create_pipeline(cmd);
        if (tasks == NULL) continue;
        run_pipeline(tasks);
        //print_pipeline(tasks);
        free_pipeline(tasks);
        free(cmd);
    } while (true);

    printf("\nGood bye!\n\n");
}
