#ifndef HASH_TABLE_LIB_H
#define HASH_TABLE_LIB_H

#include "global.h"

enum {
    ENTRY_EMPTY     = 0,
    ENTRY_USED      = 1,
    ENTRY_DELETED   = 2
};

typedef struct HashEntry HashEntry;
typedef struct HashTableObject HashTableObject;
struct HashEntry {
    ubyte_t*     key;
    keysize_t   key_size;
    hash_t      key_hash;
    flag_t      flag;
};
struct HashTableObject {
    HashEntry*  hash_entry_arr;
    int         hash_table_size;
    int         entry_count;
    int         nonempty_entry_count; // \
    # Count of entries that has ever been used (aka with flag ENTRY_DELETED/ENTRY_USED)
};



#define INIT_HTOBJ_FAILED   -1
int InitHashTableObject(HashTableObject* pObj, int MaxEntryCount);
void DestroyHashTableObject(HashTableObject* pObj);

#define INSERT_ENTRY_FAILED -1
#define HASH_TABLE_FULL     -2
hidx_t insert_entry(HashTableObject* pObj, const ubyte_t* key, keysize_t key_size);

#define KEY_NOT_FOUND -1
#define SEARCH_KEY_FAILED -2
hidx_t search_key(HashTableObject* pObj, const ubyte_t* key, keysize_t key_size);

#define DEL_KEY_FAILED -2
#define DEL_KEY_SUCCESS 0
hidx_t delete_entry(HashTableObject* pObj, const ubyte_t* key, keysize_t key_size);


#endif