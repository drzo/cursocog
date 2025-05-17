/*
 * getopt.h
 *
 * Windows implementation of getopt function headers
 *
 * Copyright (C) 2025 OpenCog Foundation
 * All Rights Reserved
 */

#ifndef _OPENCOG_GETOPT_H
#define _OPENCOG_GETOPT_H

#ifdef __cplusplus
extern "C" {
#endif

extern int opterr;
extern int optind;
extern int optopt;
extern char *optarg;

int getopt(int argc, char *const argv[], const char *optstring);

struct option {
    const char *name;
    int has_arg;
    int *flag;
    int val;
};

#define no_argument       0
#define required_argument 1
#define optional_argument 2

int getopt_long(int argc, char *const argv[],
                const char *optstring,
                const struct option *longopts, int *longindex);

#ifdef __cplusplus
}
#endif

#endif // _OPENCOG_GETOPT_H 