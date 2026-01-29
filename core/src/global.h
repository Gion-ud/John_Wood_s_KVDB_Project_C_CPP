#ifndef GLOBAL_H
#define GLOBAL_H

// Explicitly ban BE
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    #error "BE systems are NOT supported"
#elif defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN) || defined(__ARMEB__)
    #error "BE systems are NOT supported"
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stddef.h>
#include <ctype.h>
#include <dirent.h>

#define print_err_msg(...) fprintf(stderr, __VA_ARGS__)
#define printerrf(...) fprintf(stderr, __VA_ARGS__)

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


typedef signed short    word_t;
typedef unsigned short  uword_t;

typedef signed int      long_t;
typedef unsigned int    ulong_t;
typedef unsigned int    uint_t;

typedef signed char     byte_t;
typedef unsigned char   ubyte_t;
typedef unsigned char   uchar_t;


typedef void*           ptr_t;
#define PRIptr          ".16llx"
//\
typedef unsigned long long  ptr_t;
typedef unsigned char       flag_t;
typedef signed long long    qword_t;
typedef unsigned long long  uqword_t;
typedef unsigned long long  ull_t;

typedef uint64_t offptr_t;

typedef uint64_t size64_t;
typedef uint32_t size32_t;
#ifndef _WIN32
typedef uint32_t off32_t;
typedef uint64_t off64_t;
#endif

#define TIME_STR_SIZE 32

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

static inline void strputc(char c, char* str) {
    size_t len = strlen(str);
    str[len++] = c;
    str[len] = '\0';
}
void conv_time_str_modptr(char (*timestr_p_p)[TIME_STR_SIZE], time_t time_var);

#endif