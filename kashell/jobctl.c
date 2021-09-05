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
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include "jobctl.h"

/* This function stops the shell process and waits for any
   child process. */
void wait_child_process() {
    int wstatus;

    if (wait(&wstatus) == -1) {
        perror("[kashel] Error waiting child process.");
        return;
    }

#ifdef DEBUG
    if (WIFEXITED(wstatus)) {
        printf("[kashell] Exited, status=%d.\n", WEXITSTATUS(wstatus));
    } else if (WIFSIGNALED(wstatus)) {
        printf("[kashell] Killed by signal %d.\n", WTERMSIG(wstatus));
    } else if (WIFSTOPPED(wstatus)) {
        printf("[kashell] Stopped by signal %d.\n", WSTOPSIG(wstatus));
    } else if (WIFCONTINUED(wstatus)) {
        printf("[kashell] Continued.\n");
    }
#endif
}

/* This function creates a child process to execute the command
   provided by task parameter. */
void run_single_process(task_t *task) {
    // Create a task child process.
    pid_t child_pid = fork();
    if (child_pid == 0) { // child process
        int ret = execvp(task->cmd, task->params);
        if (ret != 0) {
            perror("Error on command:");
        }
    } else { // kashell process
        wait_child_process();
    }
}

/* This function runs the task pipeline. Each task
   will run in a new child process. Communication
   between the processes uses the unidirectional pipe.
   If there is only one task, no channel (pipe) will be
   created. */
void run_pipeline(task_t *tasks) {
    task_t *next;
    task_t *cur = tasks;

    if (cur->next == NULL) { // Run a single task in the pipeline.
        run_single_process(cur);
    } else {
        // Run tasks in the pipeline one by one.

    }
}