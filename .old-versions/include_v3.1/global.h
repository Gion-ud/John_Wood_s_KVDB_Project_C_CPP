#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#define print_err_msg(...) fprintf(stderr, __VA_ARGS__)

//#define DEBUG

#ifdef DEBUG
#define print_dbg_msg(...) fprintf(stderr, __VA_ARGS__)
#define PRINT_DBG_MSG(...) fprintf(stderr, __VA_ARGS__);fflush(stderr)
#else
#define print_dbg_msg(...)
#define PRINT_DBG_MSG(...)
#endif


#define LOOP_RUNNING    1
#define MAX_KEY_SIZE    256
#define BUFFER_SIZE     4096
#define FILE_PATH_SIZE  256


typedef unsigned long long  hash_t;
typedef int                 hidx_t;
typedef unsigned int        keysize_t;
//\
typedef unsigned char       byte_t;
typedef uint8_t             byte_t;
typedef void*               ptr_t;
#define PRIptr              ".16llx"
//\
typedef unsigned long long  ptr_t;
typedef unsigned char       flag_t;

typedef unsigned long long  uqword_t;

#define ESC "\x1b"
//;[
#define COLOUR_BLACK   "[30m"
#define COLOUR_RED     "[31m"
#define COLOUR_GREEN   "[32m"
#define COLOUR_YELLOW  "[33m"
#define COLOUR_BLUE    "[34m"
#define COLOUR_MAGENTA "[35m"
#define COLOUR_CYAN    "[36m"
#define COLOUR_WHITE   "[37m"
#define RESET_COLOUR   "[0m"


#endif