#ifndef HASH_TABLE_LIB_H
#define HASH_TABLE_LIB_H

#include "global_utils.h"
#include "hash_func_module.h"


#define HT_ENTRY_SIZE       sizeof(struct _ht_entry)
#define HT_OBJ_SIZE         sizeof(struct _ht_obj)

typedef struct _ht_obj      HTObject;
typedef struct _ht_entry    HashTableEntry;
typedef struct _ht_slot     HashTableSlot;

struct _ht_obj {
    HashTableSlot  *ht;
    size_t          ht_cap;
    size_t          ht_size;
};
struct _ht_entry {
    hash_t  key_hash;
    ubyte_t key_len;
    ubyte_t val_len;
    void   *key_data;
    void   *val_data;
};

typedef struct DataBuffer {
    size_t data_len;
    ubyte_t *data;
} DataBuffer;

HTObject *HASH_TABLE_LIB_NewHashTableObject(int ht_cap);
void HASH_TABLE_LIB_DestroyHashTableObject(HTObject *ht_obj);
int HASH_TABLE_LIB_InsertHashTableEntry(HTObject *ht_obj, const HashTableEntry *ht_entry);
DataBuffer *HASH_TABLE_LIB_GetHashTableEntry(HTObject *ht_obj, void *key, size_t key_len);
void HASH_TABLE_LIB_DestroyDataBuffer(DataBuffer *buf);
int HASH_TABLE_LIB_DeleteHashTableEntry(HTObject *ht_obj, void *key, size_t key_len);
HTObject* HASH_TABLE_LIB_ResizeHashTable(HTObject **ht_obj, int new_cap);

#endif