#include "hash_table_lib.h"

typedef HashTableEntry* HashTable;
typedef struct _ht_obj HTObject;

struct _ht_entry {
    hash_t  key_hash;
    ubyte_t key_len;
    ubyte_t val_len;
    void   *key_data;
    void   *val_data;

    struct _ht_entry *next;
};

struct _ht_obj {
    HashTable   ht;
    size_t      ht_cap;
    size_t      ht_size;
};

HTObject *HASH_TABLE_LIB_NewHashTableObject(int ht_cap) {
    HTObject *ht_obj = (HTObject*)malloc(HT_OBJ_SIZE);
    if (!ht_obj) {
        print_err_msg("(HTObject*)malloc(HT_OBJ_SIZE) failed\n");
        return NULL;
    }
    ht_obj->ht_cap = ht_cap;
    ht_obj->ht_size = 0;
    ht_obj->ht = (HashTableEntry*)malloc(ht_cap * sizeof(hash_t));

    if (!ht_obj->ht) {
        print_err_msg("(HashTableEntry*)malloc(ht_cap * sizeof(hash_t)) failed\n");
        goto HASH_TABLE_LIB_NewHashTableObject_failed_cleanup;
    }

    return ht_obj;
HASH_TABLE_LIB_NewHashTableObject_failed_cleanup:
    if (!ht_obj) return NULL;
    if (ht_obj->ht) {
        free(ht_obj->ht); ht_obj->ht = NULL;
    }
    return NULL;
}

int HASH_TABLE_LIB_ResizeHashTable(HTObject *ht_obj, int new_cap) {

HASH_TABLE_LIB_ResizeHashTable_failed_cleanup:
    return -1
}

int HASH_TABLE_LIB_InsertHashTableEntry(HTObject *ht_obj, const HashTableEntry *ht_entry) {
    if (!ht_obj) return -1;
    if (ht_obj->ht_size > ht_obj->ht_cap) {
        int ret = HASH_TABLE_LIB_ResizeHashTable(ht_obj, ht_obj->ht_size * 2);
        if (ret < 0) return -1;
    }

    int h_idx = ht_entry->key_hash % ht_obj->ht_cap;
    if (ht_obj->ht[h_idx].next != NULL) {
        
    }

    ht_obj->ht[ht_obj->ht_size];


    ht_obj->ht_size++;
}