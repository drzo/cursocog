/*
 * winunistd.h
 *
 * Windows implementation of UNIX-specific functions
 *
 * Copyright (C) 2025 OpenCog Foundation
 * All Rights Reserved
 */

#ifndef _OPENCOG_WINUNISTD_H
#define _OPENCOG_WINUNISTD_H

#ifdef WIN32

#include <Windows.h>
#include <io.h>
#include <direct.h>
#include <process.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

// UNIX to Windows directory handling
#define mkdir(path, mode) _mkdir(path)
#define chdir _chdir
#define getcwd _getcwd

// File descriptors
#define close _close
#define read _read
#define write _write
#define open _open
#define dup _dup
#define dup2 _dup2

// Process handling
#define getpid _getpid
#define __getpid _getpid

// File functions
#define unlink _unlink
#define lseek _lseek

// Sleep functions - Windows sleep is in milliseconds
inline unsigned int sleep(unsigned int seconds) {
    Sleep(seconds * 1000);
    return 0;
}

inline void usleep(unsigned long useconds) {
    Sleep(useconds / 1000);
}

// Timeval structure
#ifndef _TIMEVAL_DEFINED
#define _TIMEVAL_DEFINED
struct timeval {
    long tv_sec;
    long tv_usec;
};
#endif

// Timezone structure
struct timezone {
    int tz_minuteswest;
    int tz_dsttime;
};

// gettimeofday implementation
inline int gettimeofday(struct timeval* tp, void* tzp) {
    // Windows counts time in milliseconds since 1/1/1970
    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime(&system_time);
    SystemTimeToFileTime(&system_time, &file_time);

    time = ((uint64_t)file_time.dwHighDateTime << 32) | file_time.dwLowDateTime;

    // Windows file time is 100-nanosecond intervals since January 1, 1601.
    // Convert to Unix time by subtracting epoch difference (in seconds)
    // 11644473600 is the number of seconds between 1/1/1601 and 1/1/1970
    time -= 116444736000000000ULL;

    // Now convert to microseconds by dividing by 10
    time /= 10;

    tp->tv_sec = (long)(time / 1000000UL);
    tp->tv_usec = (long)(time % 1000000UL);

    return 0;
}

#endif // WIN32

#endif // _OPENCOG_WINUNISTD_H 