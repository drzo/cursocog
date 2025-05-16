/*
 * opencog/util/platform.h
 *
 * Copyright (C) 2002-2007 Novamente LLC
 * All Rights Reserved
 *
 * Written by Moshe Looks <moshe@metacog.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License v3 as
 * published by the Free Software Foundation and including the exceptions
 * at http://opencog.org/wiki/Licenses
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program; if not, write to:
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _OPENCOG_PLATFORM_H
#define _OPENCOG_PLATFORM_H

#include "cpp_keywords.h"

#ifdef WIN32

#pragma warning(disable:4290)

#define strcasecmp _stricmp

// Only define snprintf as _snprintf for older Visual C++ compilers
#if defined(_MSC_VER) && (_MSC_VER < 1900)
  #define snprintf _snprintf
#endif

// Include our Windows compatibility headers
#include "winunistd.h"
#include "winstrings.h"
#include "getopt.h"

#endif // WIN32

#include <stdio.h>
#include <string.h>
#include <string>
#include <stdint.h>
#include <cmath> // For std::round, rint etc.

#ifdef __APPLE__
char*              __strtok_r(char *s1, const char *s2, char **lasts);
#endif

#ifdef WIN32_NOT_UNIX

// Don't redefine functions that are now part of standard C++
#if !defined(_MSC_VER) || (_MSC_VER < 1800) // Visual Studio 2013 and earlier
int                round(float x);
double             rint(double nr);
unsigned long long atoll(const char *str);
#endif

// These are now handled in the winunistd.h and winstrings.h files
#if !defined(WIN32)
char*              __strtok_r(char *s1, const char *s2, char **lasts);
int                gettimeofday(struct timeval* tp, void* tzp);
void               usleep(unsigned useconds);
int                __getpid(void);
int                __dup2(int, int);
unsigned int       sleep(unsigned seconds);
#endif

#endif // ~WIN32_NOT_UNIX

namespace opencog
{
/** \addtogroup grp_cogutil
 *  @{
 */

//! Return the total amount of heap allocated (according to sbrk, on unix).
size_t getMemUsage();

//! Return the total number of bytes of physical RAM installed.
uint64_t getTotalRAM();

//! Return the total number of free bytes available in RAM (excluding OS caches)
uint64_t getFreeRAM();

//! Return the OS username
const char* getUserName();

void set_thread_name(const char* name);

/** @}*/
} // namespace opencog

#endif // _OPENCOG_PLATFORM_H 