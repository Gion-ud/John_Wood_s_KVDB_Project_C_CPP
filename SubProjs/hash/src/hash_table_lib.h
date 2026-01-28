#ifndef HASH_TABLE_LIB_H
#define HASH_TABLE_LIB_H

#include "global_utils.h"

typedef struct HashEntry HashEntry;
typedef struct HashTableObject HashTableObject;


struct HashEntry {
    byte_t*     key;
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

hash_t HTLib_FNV_1a_hash(const byte_t* key, keysize_t size);

#define INIT_HTOBJ_FAILED   -1
int InitHashTableObject(HashTableObject* pObj, int MaxEntryCount);
void DestroyHashTableObject(HashTableObject* pObj);

#define INSERT_ENTRY_FAILED -1
#define HASH_TABLE_FULL     -2
hidx_t HashTableObject_insert_entry(HashTableObject* pObj, const byte_t* key, keysize_t key_size);

#define KEY_NOT_FOUND -1
#define SEARCH_KEY_FAILED -2
hidx_t HashTableObject_search_key(HashTableObject* pObj, const byte_t* key, keysize_t key_size);

#define DEL_KEY_FAILED -2
#define DEL_KEY_SUCCESS 0
hidx_t HashTableObject_delete_entry(HashTableObject* pObj, const byte_t* key, keysize_t key_size);

int HashTableObject_get_key_hash(HashTableObject* pObj, hash_t* key_hash, const byte_t* key, keysize_t key_size);

#endif