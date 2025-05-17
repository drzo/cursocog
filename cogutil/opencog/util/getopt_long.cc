#include "platform.h"
/*
 * getopt_long.cc
 *
 * Windows implementation of getopt_long function
 *
 * Copyright (C) 2025 OpenCog Foundation
 * All Rights Reserved
 */

#include <string.h>
#include <stdio.h>

struct option {
    const char *name;
    int has_arg;
    int *flag;
    int val;
};

#define no_argument       0
#define required_argument 1
#define optional_argument 2

extern char *optarg;
extern int optind;
extern int opterr;
extern int optopt;

static int match_longopts(char *arg, const char *name)
{
    while (*arg && *name && *arg == *name) {
        arg++;
        name++;
    }
    return (*name == '\0' && *arg == '\0');
}

int getopt_long(int argc, char *const argv[],
                const char *optstring,
                const struct option *longopts, int *longindex)
{
    static char has_equal[2] = {0, '='};
    static char empty_string[1] = {0};
    
    if (optind >= argc)
        return -1;
    
    char *current_argv = argv[optind];
    
    // Handle standard getopt case
    if (current_argv[0] == '-' && current_argv[1] != '-') {
        return getopt(argc, argv, optstring);
    }
    
    // Handle '--' case which means end of options
    if (strcmp(current_argv, "--") == 0) {
        optind++;
        return -1;
    }
    
    // Not an option at all
    if (current_argv[0] != '-')
        return -1;
    
    // Handle long options (starting with '--')
    if (current_argv[0] == '-' && current_argv[1] == '-') {
        char *arg = current_argv + 2;
        char *equal_pos = strchr(arg, '=');
        int option_index = 0;
        
        // If we have an equal sign, temporarily replace it with null
        if (equal_pos) {
            *equal_pos = '\0';
        }
        
        // Search through longopts for a match
        for (option_index = 0; longopts[option_index].name; option_index++) {
            if (match_longopts(arg, longopts[option_index].name)) {
                // Match found
                if (longindex)
                    *longindex = option_index;
                
                // Restore the equal sign if we had one
                if (equal_pos) {
                    *equal_pos = '=';
                }
                
                // Handle arguments
                if (longopts[option_index].has_arg == required_argument ||
                    longopts[option_index].has_arg == optional_argument) {
                    if (equal_pos) {
                        // Argument is after the equal sign
                        optarg = equal_pos + 1;
                    } else if (longopts[option_index].has_arg == required_argument) {
                        // Argument is the next argv
                        if (optind + 1 >= argc) {
                            if (opterr)
                                fprintf(stderr, "%s: option '--%s' requires an argument\n",
                                        argv[0], longopts[option_index].name);
                            optopt = longopts[option_index].val;
                            return '?';
                        }
                        optarg = argv[++optind];
                    } else {
                        // Optional argument not provided
                        optarg = NULL;
                    }
                } else {
                    // No argument expected
                    if (equal_pos) {
                        if (opterr)
                            fprintf(stderr, "%s: option '--%s' doesn't allow an argument\n",
                                    argv[0], longopts[option_index].name);
                        optopt = longopts[option_index].val;
                        return '?';
                    }
                    optarg = NULL;
                }
                
                optind++;
                
                if (longopts[option_index].flag) {
                    *(longopts[option_index].flag) = longopts[option_index].val;
                    return 0;
                } else {
                    return longopts[option_index].val;
                }
            }
        }
        
        // Restore the equal sign if we had one
        if (equal_pos) {
            *equal_pos = '=';
        }
        
        // Unknown option
        if (opterr)
            fprintf(stderr, "%s: unrecognized option '%s'\n", argv[0], current_argv);
        optind++;
        return '?';
    }
    
    return -1;
} 
