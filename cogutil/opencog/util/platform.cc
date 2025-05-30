/*
 * opencog/util/platform.cc
 *
 * Copyright (C) 2002-2007 Novamente LLC
 * Copyright (C) 2008 by OpenCog Foundation
 * All Rights Reserved
 *
 * Written by Gustavo Gama <moshe@metacog.org>
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

#include "platform.h"
#include <stdlib.h>

namespace opencog {

const char* getUserName() {
    const char* username = getenv("LOGNAME");
    if (username == NULL)
        username = getenv("USER");
    if (username == NULL)
        username = "unknown_user";
    return username;
}

} // ~namespace opencog

// ===========================================

#ifdef __APPLE__

#include <sys/timeb.h>
#include <math.h>

#ifndef HAVE_STRTOK_R
#define HAVE_STRTOK_R 1

char* __strtok_r(char *s1, const char *s2, char **lasts)
{
    char *ret;

    if (s1 == NULL)
        s1 = *lasts;
    while (*s1 && strchr(s2, *s1))
        ++s1;
    if (*s1 == '\0')
        return NULL;
    ret = s1;
    while (*s1 && !strchr(s2, *s1))
        ++s1;
    if (*s1)
        *s1++ = '\0';
    *lasts = s1;
    return ret;
}

#endif /* HAVE_STRTOK_R */
#endif /* __APPLE__ */

// ======================================================
#ifdef WIN32_NOT_UNIX

#include <sys/timeb.h>
#include <winsock2.h>
#include <process.h>
#include <math.h>
#include <io.h>

int round(float x)
{
    return ((x -(int)(x)) < 0.5 ) ? (int)x : (int)x + 1;
}

int gettimeofday(struct timeval* tp, void* tzp)
{
    struct _timeb timebuffer;
    _ftime(&timebuffer);
    tp->tv_sec = (long) timebuffer.time;
    tp->tv_usec = timebuffer.millitm * 1000;
    /* 0 indicates that the call succeeded. */
    return 0;
}

void usleep(unsigned useconds)
{
    // Sleep is in milliseconds
    // If 0 is passed to Sleep()
    // It skips rest of thread scheduled time
    // This is the best achievable with Millisecond
    // resolution
    Sleep((int)(useconds / 1000));
}

unsigned sleep(unsigned seconds)
{
    Sleep(seconds * 1000);
    return 0;
}

#ifndef HAVE_STRTOK_R
#define HAVE_STRTOK_R 1

char* __strtok_r(char *s1, const char *s2, char **lasts)
{
    char *ret;

    if (s1 == NULL)
        s1 = *lasts;
    while (*s1 && strchr(s2, *s1))
        ++s1;
    if (*s1 == '\0')
        return NULL;
    ret = s1;
    while (*s1 && !strchr(s2, *s1))
        ++s1;
    if (*s1)
        *s1++ = '\0';
    *lasts = s1;
    return ret;
}

#endif /* HAVE_STRTOK_R */

int __getpid(void)
{
    return _getpid();
}

double rint(double nr)
{
    double f = floor(nr);
    double c = ceil(nr);
    return (((c -nr) >= (nr - f)) ? f : c);
}

int __dup2(int fd1, int fd2)
{
    return _dup2(fd1, fd2);
}

unsigned long long atoll(const char *str)
{
    unsigned long long la = 0;
    sscanf(str, "%Lu", &la);
    return la;
}

#endif // WIN32_NOT_UNIX

// ==========================================================

#include <stdlib.h>
#include <unistd.h>   // for sbrk(), sysconf()

// Return memory usage per sbrk system call.
size_t opencog::getMemUsage()
{
    static void *old_sbrk = 0;
    void *p = sbrk(0);
    if (old_sbrk == 0 || old_sbrk > p)
    {
        old_sbrk = p;
        return 0;
    }
    size_t diff = (size_t)p - (size_t)old_sbrk;
    return diff;
}

#ifdef __APPLE__
#include <sys/sysctl.h>
#include <sys/types.h>
#include <pthread.h>

uint64_t opencog::getTotalRAM()
{
   int mib[2];
   uint64_t physmem;
   size_t len;

   mib[0] = CTL_HW;
   mib[1] = HW_MEMSIZE;
   len = sizeof(physmem);
   sysctl(mib, 2, &physmem, &len, NULL, 0);
   return physmem;
}

uint64_t opencog::getFreeRAM()
{
    return getTotalRAM() - getMemUsage();
}

void opencog::set_thread_name(const char* name)
{
    pthread_setname_np(name);
}

#else // __APPLE__

// If not Apple, then Linux.
#include <sys/sysinfo.h>
#include <sys/prctl.h>

uint64_t opencog::getTotalRAM()
{
    // return getpagesize() * get_phys_pages();
    return getpagesize() * sysconf(_SC_PHYS_PAGES);
}

uint64_t opencog::getFreeRAM()
{
    // return getpagesize() * get_avphys_pages();
    return getpagesize() * sysconf(_SC_AVPHYS_PAGES);
}

void opencog::set_thread_name(const char* name)
{
    prctl(PR_SET_NAME, name, 0, 0, 0);
}
#endif // __APPLE__

#include <cmath>
#include <string>

#ifdef WIN32
#pragma warning(disable: 4273) // Inconsistent dll linkage
#endif

#ifndef WIN32
#include <unistd.h>
#include <sys/time.h>
#endif

#ifdef __APPLE__
#include <mach/mach.h>
#endif // __APPLE__

#ifdef __sun__
#include <sys/types.h>
#include <sys/processor.h>
#include <sys/procset.h>
#endif // __sun__
