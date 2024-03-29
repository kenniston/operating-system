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

#define CHANGE_DIR_CMD "cd"

/* This function stops the shell process and waits for any
   child process. */
void wait_child_process(int pid) {
    int wstatus;

    if (waitpid(pid, &wstatus, 0) == -1 && errno != EINTR) {
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
void configure_stdinout(task_t *task) {
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
                perror("Error changing default output file descriptor.");
            }
            close(fd[0]);
        }
    }

    if (infile) {
        fd[1] = open(task->infile, O_RDONLY);
        if (fd[1] == -1) {
            perror("Error reading the input file.");
        } else {
            int ret = dup2(fd[1], STDIN_FILENO);
            if (ret == -1) {
                perror("Error changing default input file descriptor.");
            }
            close(fd[1]);
        }
    }
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
        // Create a new process group
        pid_t pgid = getpid();
        setpgid(pgid, pgid);

        // Configure input and output files.
        configure_stdinout(task);

        // Change the image on the child process.
        int ret = execvp(task->cmd, task->params);
        if (ret != 0) {
            perror("Error on command:");
        }
    } else { // kashell process
        // Create a new process group.
        // Setting process group on parent (shell)
        // is to avoid race condition between parent
        // and child process.
        // https://www.gnu.org/software/libc/manual/html_node/Launching-Jobs.html
        pid_t pgid = getpid();
        setpgid(pgid, pgid);


        wait_child_process(child_pid);
    }
}

/* This function creates a chained task execution. Ex.:
   ls -la | grep kashell
   ls -la | grep kashell > outfile.txt
   echo test | wc
   ps -aux | grep systemd
   ps -aux | grep systemd | wc -l

   Warn: to debug the child process, set a breakpoint before the
         fork and run the following commands in the gdb console:
            1) set follow-fork-mode child;
            2) set detach-on-fork off;
            3) Continue debugging.
*/
void run_process_pipeline(task_t *tasks) {
    task_t *cur = tasks;
    int index = 0;
    int lastfd[2] = {0, 0};
    pid_t pgid = 0;

    // Creates the pipe file descriptors for interprocess communication.
    // The inoutfd[0] is the reader and inoutfd[1] is the writter.
    int inoutfd[2];
    if (pipe(inoutfd) == -1) {
        perror("Error creating task pipe.");
        return;
    }
    // Perform the tasks.
    while (cur) {
        // Create a task child process.
        pid_t child_pid = fork();
        if (child_pid == 0) { // child process
            // Create a new process group
            if (pgid == 0) pgid = child_pid;
            setpgid(child_pid, pgid);

            // Closes the write file descriptor in the child process
            // if this task is the last one. Otherwise, set the writer
            // file descriptor to the child process.
            if (cur->next != NULL) {
                if (dup2(inoutfd[PIPE_WRITE], STDOUT_FILENO) == -1) {
                    perror("Error setting STDOUT for child process.");
                }
            } else {
                bool infile = strcmp(cur->infile, "") != 0;
                if (infile) {
                    errno = EINVAL;
                    perror("Input file is not allowed for chained tasks.");
                    return;
                }
                configure_stdinout(cur);
            }
            close(inoutfd[PIPE_READ]);
            close(inoutfd[PIPE_WRITE]);

            // Reads from STDOUT after the first command in the pipeline.
            if (index > 0) {
                if (dup2(lastfd[PIPE_READ], STDIN_FILENO) == -1) {
                    perror("Error setting STDIN for child process.");
                }
            }
            close(lastfd[PIPE_READ]);
            close(lastfd[PIPE_WRITE]);

            // Change the image on the child process.
            int ret = execvp(cur->cmd, cur->params);
            if (ret != 0) {
                perror("Error on command:");
            }
        }
        // Create a new process group.
        // Setting process group on parent (shell)
        // is to avoid race condition between parent
        // and child process.
        // https://www.gnu.org/software/libc/manual/html_node/Launching-Jobs.html
        if (pgid == 0) pgid = child_pid;
        setpgid(child_pid, pgid);

        // Closes the last file descriptors. If the descriptors
        // remain open, the child process will be blocked forever.
        if (lastfd[PIPE_READ] > 0) {
            close(lastfd[PIPE_READ]);
        }
        if (lastfd[PIPE_WRITE] > 0) {
            close(lastfd[PIPE_WRITE]);
        }

        wait_child_process(child_pid);

        // Configure next task if its exists.
        index++;
        cur = cur->next;
        if (cur != NULL) {
            // Backup the last file descriptors
            lastfd[PIPE_READ] = inoutfd[PIPE_READ];
            lastfd[PIPE_WRITE] = inoutfd[PIPE_WRITE];

            // Create a new file descriptor pair for the next command.
            if (pipe(inoutfd) == -1) {
                perror("Error creating task pipe.");
                return;
            }
        }
    }
}

/* This function runs the task pipeline. Each task
   will run in a new child process. Communication
   between the processes uses the unidirectional pipe.
   If there is only one task, no channel (pipe) will be
   created. */
void run_pipeline(task_t *tasks) {
    if (strcasecmp(tasks->cmd, CHANGE_DIR_CMD) == 0) {
        chdir(tasks->params[1]);
        return;
    }
    if (tasks->next == NULL) { // Run a single task in the pipeline.
        run_single_process(tasks);
    } else { // Run tasks in the pipeline one by one.
        run_process_pipeline(tasks);
    }
}