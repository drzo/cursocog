#include "platform.h"
/* A replacement function, for systems that lack strndup.

   Copyright (C) 1996, 1997, 1998, 2001, 2002, 2003, 2005, 2006, 2007
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.  */

#include <stdlib.h>
#include <string.h>

#include "strnlen.h"

char *
strndup (char const *s, size_t n)
{
  size_t len = strnlen (s, n);
  char *x = (char*)malloc (len + 1);

  if (x == NULL)
    return NULL;

  x[len] = '\0';
  return (char*) memcpy (x, s, len);
}
