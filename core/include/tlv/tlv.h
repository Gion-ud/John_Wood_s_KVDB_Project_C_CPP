// kvdb.h
#ifndef TLV_H
#define TLV_H

#include "global.h"
#include "typeflags.h"
#include "export.h"

#define TLV_API LIB_API

typedef struct _tlv_obj TLVDataObject;
struct _tlv_obj {
    ulong_t     type;
    size32_t    len;
    void       *data;
};


#endif