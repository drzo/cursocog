#include "platform.h"
/* Find the length of STRING, but scan at most MAXLEN characters.
   Copyright (C) 2005, 2006, 2007 Free Software Foundation, Inc.
   Written by Simon Josefsson.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

/* Find the length of STRING, but scan at most MAXLEN characters.
   If no '\0' terminator is found in that many characters, return MAXLEN.  */

#include <stdlib.h>
#include <string.h>

size_t
strnlen (const char *string, size_t maxlen)
{
  const char *end = (const char*) memchr (string, '\0', maxlen);
  return end ? (size_t) (end - string) : maxlen;
}
