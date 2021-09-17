/* jobctl.c - functions to manager the jobs created from kashell tasks. */

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
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/file.h>
#include "jobctl.h"

/* This function stops the shell process and waits for any
   child process. */
void wait_child_process() {
    int wstatus;

    if (wait(&wstatus) == -1 && errno != EINTR) {
        perror("[kashel] Error waiting child process.");
        return;
    }

#ifdef DEBUG
    if (WIFEXITED(wstatus)) {
        printf("[kashell] Child Exited, status=%d.\n", WEXITSTATUS(wstatus));
    } else if (WIFSIGNALED(wstatus)) {
        printf("[kashell] Child Killed by signal %d.\n", WTERMSIG(wstatus));
    } else if (WIFSTOPPED(wstatus)) {
        printf("[kashell] Child Stopped by signal %d.\n", WSTOPSIG(wstatus));
    } else if (WIFCONTINUED(wstatus)) {
        printf("[kashell] Child Continued.\n");
    }
#endif
}

/* This function creates file descriptors for input and
   output file for the task. Also, it changes the standard
   output and input file descriptor in the process. */
int * configure_stdinout(task_t *task) {
    int *fd = calloc(1, sizeof(int) * 2);
    bool outfile = strcmp(task->outfile, "") != 0;
    bool infile = strcmp(task->infile, "") != 0;

    if (outfile) {
        fd[0] = open(task->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (fd[0] == -1) {
            perror("Error creating the output file.");
        } else {
            int ret = dup2(fd[0], STDOUT_FILENO);
            if (ret == -1) {
                perror("Error changing default output file descriptor");
            }
        }
    }

    if (infile) {
        fd[1] = open(task->infile, O_RDONLY);
        if (fd[1] == -1) {
            perror("Error reading the input file.");
        } else {
            int ret = dup2(fd[1], STDIN_FILENO);
            if (ret == -1) {
                perror("Error changing default input file descriptor");
            }
        }
    }
    return fd;
}

/* This function creates a child process to execute the command
   provided by task parameter.
   Warn: to debug the child process, set a breakpoint before the
         fork and run the following commands in the gdb console:
            1) set follow-fork-mode child;
            2) set detach-on-fork off;
            3) Continue debugging.
*/
void run_single_process(task_t *task) {
    // Create a task child process.
    pid_t child_pid = fork();
    if (child_pid == 0) { // child process
        int *fd = configure_stdinout(task);
        int ret = execvp(task->cmd, task->params);
        if (ret != 0) {
            perror("Error on command:");
        }
        // Close files in child process if execvp fails.

    } else { // kashell process
        wait_child_process();
    }
}

void run_process_pipeline(task_t *tasks) {
    task_t *cur = tasks;
    while (cur) {
        pid_t child_pid = fork();
        if (child_pid == 0) { // child process

        } else { // kashell process
            wait_child_process();
        }
        cur = cur->next;
    }
}

/* This function runs the task pipeline. Each task
   will run in a new child process. Communication
   between the processes uses the unidirectional pipe.
   If there is only one task, no channel (pipe) will be
   created. */
void run_pipeline(task_t *tasks) {
    if (tasks->next == NULL) { // Run a single task in the pipeline.
        run_single_process(tasks);
    } else { // Run tasks in the pipeline one by one.
        run_process_pipeline(tasks);
    }
}