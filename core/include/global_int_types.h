#pragma once
#include <stddef.h>
#include <stdint.h>

typedef unsigned long long  hash_t;
typedef int                 hidx_t;
typedef unsigned int        keysize_t;


typedef signed char         byte_t;
typedef unsigned char       ubyte_t;
typedef unsigned char       uchar_t;

typedef signed short        short_t;
typedef unsigned short      ushort_t;
typedef signed short        word_t;
typedef unsigned short      uword_t;

typedef signed int          dword_t;
typedef unsigned int        udword_t;
typedef signed int          long_t;
typedef unsigned int        ulong_t;

typedef signed long long    longlong_t;
typedef signed long long    qword_t;
typedef unsigned long long  ulonglong_t;
typedef unsigned long long  uqword_t;

typedef ulong_t             size32_t;
typedef size_t              size64_t;

#ifndef __MINGW32__
typedef long_t              off32_t;
typedef longlong_t          off64_t;
#endif

typedef ulong_t             uoff32_t;
typedef ulonglong_t         uoff64_t;


typedef unsigned long long  ull_t;
