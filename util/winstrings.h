/*
 * winstrings.h
 *
 * Windows implementation of UNIX string functions
 *
 * Copyright (C) 2025 OpenCog Foundation
 * All Rights Reserved
 */

#ifndef _OPENCOG_WINSTRINGS_H
#define _OPENCOG_WINSTRINGS_H

#ifdef WIN32

#include <string.h>
#include <stdlib.h>

// strtok_r implementation for Windows
inline char* __strtok_r(char* str, const char* delim, char** saveptr) {
    if (str == NULL) {
        str = *saveptr;
    }
    
    if (str == NULL) {
        return NULL;
    }
    
    str += strspn(str, delim);
    if (*str == '\0') {
        *saveptr = NULL;
        return NULL;
    }
    
    char* end = str + strcspn(str, delim);
    if (*end == '\0') {
        *saveptr = NULL;
    } else {
        *end = '\0';
        *saveptr = end + 1;
    }
    
    return str;
}

// strcasecmp is called _stricmp in Windows
#ifndef strcasecmp
#define strcasecmp _stricmp
#endif

// strncasecmp is called _strnicmp in Windows
#ifndef strncasecmp
#define strncasecmp _strnicmp
#endif

#endif // WIN32

#endif // _OPENCOG_WINSTRINGS_H 