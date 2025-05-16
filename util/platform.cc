/*
 * opencog/util/platform.cc
 *
 * Copyright (C) 2002-2007 Novamente LLC
 * Copyright (C) 2008 by OpenCog Foundation
 * All Rights Reserved
 *
 * Written by Gustavo Gama <gama@vettalabs.com>
 *            Linas Vepstas <linasvepstas@gmail.com>
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

#include <cmath>
#include <stdlib.h>
#include <string>
#ifdef WIN32
#include <Windows.h>
#include <psapi.h>  // For GetPerformanceInfo, PERFORMANCE_INFORMATION
#else
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

// Fixes Solaris GCC link error
#if defined(sun) || defined(__sun)
using std::rint;
using std::round;
#endif

namespace opencog {

#ifdef WIN32

size_t getMemUsage() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    return (size_t)pmc.WorkingSetSize;
}

uint64_t getTotalRAM() {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return (uint64_t)status.ullTotalPhys;
}

uint64_t getFreeRAM() {
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return (uint64_t)status.ullAvailPhys;
}

const char* getUserName() {
    static char username[UNLEN + 1] = {0};
    DWORD username_len = UNLEN + 1;
    GetUserName(username, &username_len);
    return username;
}

void set_thread_name(const char* name) {
    // This uses a Windows-specific trick
    typedef struct tagTHREADNAME_INFO {
        DWORD dwType;     // Must be 0x1000
        LPCSTR szName;    // Pointer to name (in user addr space)
        DWORD dwThreadID; // Thread ID (-1=caller thread)
        DWORD dwFlags;    // Reserved for future use, must be zero
    } THREADNAME_INFO;

    THREADNAME_INFO info;
    info.dwType = 0x1000;
    info.szName = name;
    info.dwThreadID = -1;
    info.dwFlags = 0;

    __try {
        RaiseException(0x406D1388, 0, sizeof(info) / sizeof(DWORD), (ULONG_PTR*)&info);
    } __except (EXCEPTION_CONTINUE_EXECUTION) {
    }
}

#else // !WIN32 (Unix/Linux implementation)

#if defined(__linux__) || defined(ANDROID) || defined(__ANDROID__)
size_t getMemUsage() {
    unsigned long resident = 0;
    FILE* statm = fopen("/proc/self/statm", "r");
    if (statm == NULL) return 0;
    int count = fscanf(statm, "%*d %lu", &resident);
    fclose(statm);
    if (count != 1) return 0;
    return resident * getpagesize();
}

uint64_t getTotalRAM() {
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
}

uint64_t getFreeRAM() {
    long pages = sysconf(_SC_AVPHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
}
#endif

#if defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
size_t getMemUsage() {
    /* Not implemented */
    return 0;
}

uint64_t getTotalRAM() {
    /* Not implemented */
    return 0;
}

uint64_t getFreeRAM() {
    /* Not implemented */
    return 0;
}
#endif

#ifdef __APPLE__
size_t getMemUsage() {
    struct task_basic_info t_info;
    mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
    
    if (KERN_SUCCESS != task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count)) {
        return 0;
    }
    return t_info.resident_size;
}

uint64_t getTotalRAM() {
    int mib[2];
    int64_t physical_memory;
    size_t length;
    
    mib[0] = CTL_HW;
    mib[1] = HW_MEMSIZE;
    length = sizeof(int64_t);
    sysctl(mib, 2, &physical_memory, &length, NULL, 0);
    
    return physical_memory;
}

uint64_t getFreeRAM() {
    vm_statistics_data_t vm_stats;
    mach_msg_type_number_t info_count = HOST_VM_INFO_COUNT;
    
    if (host_statistics(mach_host_self(), HOST_VM_INFO, (host_info_t)&vm_stats, &info_count) != KERN_SUCCESS) {
        return 0;
    }
    
    return vm_stats.free_count * getpagesize();
}
#endif

const char* getUserName() {
    static char username[256];
    auto user = getlogin();
    if (user == NULL) user = getenv("USER");
    if (user == NULL) user = getenv("LOGNAME");
    if (user == NULL) {
        strcpy(username, "unknown_user");
        return username;
    }
    strcpy(username, user);
    return username;
}

void set_thread_name(const char* name) {
#ifdef __APPLE__
    pthread_setname_np(name);
#else
    char shortened[16]; // Linux requires 16 or fewer characters
    strncpy(shortened, name, 15);
    shortened[15] = '\0';
    pthread_setname_np(pthread_self(), shortened);
#endif
}

#endif // WIN32 vs Unix implementation

} // namespace opencog
 