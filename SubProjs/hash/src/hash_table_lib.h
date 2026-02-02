#ifndef HASH_TABLE_LIB_H
#define HASH_TABLE_LIB_H

#include "global_utils.h"
#include "hash_func_module.h"


struct _HashTableEntry {
    hash_t  key_hash;
    ubyte_t key_len;
    ubyte_t val_len;
    void   *key;
    void   *val;

    struct _HashTableEntry *next;
};

#endif