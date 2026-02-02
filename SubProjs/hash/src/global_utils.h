#ifndef GLOBAL_UTILS_H
#define GLOBAL_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>

#define print_err_msg(...) fprintf(stderr, __VA_ARGS__)

//#define DEBUG

#ifdef DEBUG
    #define print_dbg_msg(...) fprintf(stderr, __VA_ARGS__)
#else
    #define print_dbg_msg(...)
#endif
#define LOOP_RUNNING 1

typedef unsigned long long  hash_t;
typedef int                 hidx_t;
typedef unsigned int        ulong_t;
typedef unsigned char       ubyte_t;
typedef unsigned char       flag_t;

typedef unsigned long long ptr_t;
#define PRIptr ".16llx"


#define MAX_KEY_SIZE 256

enum {
    ENTRY_EMPTY = 0,
    ENTRY_USED = 1,
    ENTRY_DELETED = 2
};

#endif