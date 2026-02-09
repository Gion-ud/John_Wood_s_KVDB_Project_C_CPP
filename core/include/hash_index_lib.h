#ifndef HASH_INDEX_LIB_H
#define HASH_INDEX_LIB_H

#include "global.h"
#include "hash_func_module.h"

#define INIT_BUCKET_CAP 4

typedef struct _ht_obj      HTObject;
typedef struct _ht_entry    HashTableEntry;
typedef struct _ht_slot     HashTableSlot;

enum _ht_entry_state {
    HT_ENTRY_EMPTY      = 0,
    HT_ENTRY_INUSE      = 1,
    HT_ENTRY_DELETED    = 2,
};

enum _ht_slot_state {
    HT_SLOT_EMPTY   = 0,
    HT_SLOT_INUSE   = 1,
    HT_SLOT_DELETED = 2,
};

struct _ht_obj {
    HashTableSlot *ht;
    size_t  ht_cap;
    size_t  ht_slot_used_cnt;
    size_t  ht_entry_cnt;
    size_t  ht_del_cnt;
};
struct _ht_entry {
    hash_t  key_hash;
    int     entry_id;
    ubyte_t state;
};

struct _ht_slot {
    HashTableEntry *bucket;
    size_t  bucket_cap;
    size_t  bucket_size;
    ubyte_t slot_state;
};

static inline int HASH_INDEX_LIB_HTObject_get_slot_idx(HTObject *ht_obj, hash_t key_hash) {
    int h_idx = key_hash % ht_obj->ht_cap;
    if (!ht_obj->ht[h_idx].slot_state) return -1;
    return key_hash % ht_obj->ht_cap;
}

HTObject *HASH_INDEX_LIB_HTObject_create(int ht_cap);
void HASH_INDEX_LIB_HTObject_destroy(HTObject *ht_obj);
int HASH_INDEX_LIB_HTObject_insert(HTObject *ht_obj, hash_t key_hash, ulong_t entry_id);
int HASH_INDEX_LIB_HTObject_delete(HTObject *ht_obj, hash_t key_hash, ulong_t entry_id);
HTObject* HASH_INDEX_LIB_HTObject_resize(HTObject *ht_obj, int new_cap);
HTObject* HASH_INDEX_LIB_HTObject_compact(HTObject *ht_obj);


#endif