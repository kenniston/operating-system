/* cmd.h - functions and structures used for reading and process kashell
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

#ifndef KASHELL_CMD_H
#define KASHELL_CMD_H

#define MAX_CMD_SIZE 512        // Max command line length (program to run).
#define MAX_PARAMS 255          // Max parameter length (parameters to a program).
#define MAX_OUT_FILE_PATH 255   // Max length for the output file path.
#define MAX_IN_FILE_PATH 255    // Max length for the input file path.

/* Linked list for tasks on the command line. The tasks are created
  by a command line parsing function and processed by Job Control. */
typedef struct task {
    struct task *next;
    char cmd[MAX_CMD_SIZE];
    char *params[MAX_PARAMS];
    char outfile[MAX_OUT_FILE_PATH];
    char infile[MAX_IN_FILE_PATH];
} task_t;

void run_shell();

#endif //KASHELL_CMD_H
