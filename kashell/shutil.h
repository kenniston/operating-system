/* shutil.h - shell util functions. */

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
#ifndef KASHELL_SHUTIL_H
#define KASHELL_SHUTIL_H

/* Remove all white space from the beginning and end of the string. */
void str_trim(char * s);

/* Get the last string after the token. */
char* laststr (char* myStr, char token);

#endif //KASHELL_SHUTIL_H
