/* kashel.c - KaShell, the miniature-shell. */

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
#include <stdlib.h>
#include "cmd.h"

#define clrscr() printf("\e[1;1H\e[2J")

void print_welcome() {
    clrscr();
    printf("Welcome to the KaShell.\n");
}

int main() {
    print_welcome();
    run_shell();

	return EXIT_SUCCESS;
}
