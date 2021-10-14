/* shutil.c - shell util functions. */

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

#include <stddef.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include "shutil.h"

/* Remove all white space from the beginning and end of the string. */
void str_trim(char * s) {
    if (s == NULL) return;
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

/* Get the last string after the token. */
char* laststr (char* myStr, char token) {
    char *del = &myStr[strlen(myStr)];
    while (del > myStr && *del != token) del--;
    if (*del== token) del++;
    return del;
}
