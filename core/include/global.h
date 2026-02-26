#ifndef GLOBAL_H
#define GLOBAL_H

// Explicitly ban BE
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)
    #error "BE systems are NOT supported"
#elif defined(__BIG_ENDIAN__) || defined(_BIG_ENDIAN) || defined(__ARMEB__)
    #error "BE systems are NOT supported"
#endif

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <dirent.h>

#include "global_intdef.h"

#ifdef _MSC_VER
    #include <io.h>
    #define _write(fd, buf_p, buf_size) write(fd, buf_p, buf_size)
    #define fileno(fp) _fileno(fp)
    #define STDOUT_FILENO 1
    #define STDERR_FILENO 2
#else
    #include <unistd.h>
#endif

#define print_err_msg(...) fprintf(stderr, __VA_ARGS__)
#define printerrf(...) fprintf(stderr, __VA_ARGS__)

#define DEBUG

#ifdef DEBUG
#define print_dbg_msg(...) fprintf(stderr, __VA_ARGS__);fflush(stderr)
#else
#define print_dbg_msg(...)
#define PRINT_DBG_MSG(...)
#endif


#define LOOP_RUNNING    1
#define MAX_KEY_SIZE    256
#define BUFFER_SIZE     4096
#define FILE_PATH_SIZE  256


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

char* conv_bytes_hex(const unsigned char* bytes, size_t size);
static inline void strputc(char c, char* str) {
    size_t len = strlen(str);
    str[len++] = c;
    str[len] = '\0';
}
void conv_time_str_modptr(char (*timestr_p_p)[TIME_STR_SIZE], time_t time_var);

#endif