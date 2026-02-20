// kvdb.h
#ifndef TLV_H
#define TLV_H

#include "global.h"
#include "typeflags.h"


#if defined(_MSC_VER)
    #ifdef DLL_EXPORT
        #define TLV_API __declspec(dllexport)
    #else
        #define KVDB_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__) || defined(__ICC)
    #define KVDB_API __attribute__((visibility("default")))
#else
    #define KVDB_API
#endif

typedef struct _tlv_obj TLVDataObject;
struct _tlv_obj {
    ulong_t     type;
    size32_t    len;
    void       *data;
};






#endif