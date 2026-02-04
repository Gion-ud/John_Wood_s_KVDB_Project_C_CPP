#include "hash_table_lib.h"

HTObject *HASH_TABLE_LIB_NewHashTableObject(int ht_cap) {
    HTObject *ht_obj = (HTObject*)malloc(HT_OBJ_SIZE);
    if (!ht_obj) {
        print_err_msg("(HTObject*)malloc(HT_OBJ_SIZE) failed\n");
        return NULL;
    }
    ht_obj->ht_cap = ht_cap;
    ht_obj->ht_size = 0;
    ht_obj->ht = (HashTableSlot*)calloc(ht_cap, sizeof(HashTableSlot));

    if (!ht_obj->ht) {
        print_err_msg("(HashTableEntry*)calloc(ht_cap, sizeof(HashTableEntry)) failed\n");
        goto NewHashTableObject_failed_cleanup;
    }

    for (size_t i = 0; i < ht_cap; ++i) {
        ht_obj->ht[i].bucket_cap = INIT_BUCKET_CAP;
        ht_obj->ht[i].bucket_size = 0;
        ht_obj->ht[i].bucket = (HashTableEntry*)calloc(INIT_BUCKET_CAP, HT_ENTRY_SIZE);
        if (!ht_obj->ht[i].bucket) {
            print_err_msg("(HashTableEntry*)calloc(INIT_BUCKET_CAP, HT_ENTRY_SIZE) failed\n");
            goto NewHashTableObject_failed_cleanup;
        }
    }

    return ht_obj;
NewHashTableObject_failed_cleanup:
    if (!ht_obj) return NULL;
    if (ht_obj->ht) {
        for (size_t i = 0; i < ht_obj->ht_cap; ++i) {
            if (ht_obj->ht[i].bucket) {
                free(ht_obj->ht[i].bucket);
                ht_obj->ht[i].bucket = NULL;
            }
        }
        free(ht_obj->ht); ht_obj->ht = NULL;
    }
    return NULL;
}

void HASH_TABLE_LIB_DestroyHashTableObject(HTObject *ht_obj) {
//#define ht_obj (*ht_obj_p)
    print_dbg_msg("HASH_TABLE_LIB_DestroyHashTableObject\n");
    if (!ht_obj) return;
    if (!ht_obj->ht) { free(ht_obj); return; }
    print_dbg_msg("ht_obj->ht=%p\n", ht_obj->ht);
    for (size_t i = 0; i < ht_obj->ht_cap; ++i) {
        if (ht_obj->ht[i].bucket) {
            print_dbg_msg("ht_obj->ht[%zu].bucket=%p\n", i, ht_obj->ht[i].bucket);
            print_dbg_msg("idx=%zu\n", i);
            for (size_t j = 0; j < ht_obj->ht[i].bucket_cap; j++) {
                if (ht_obj->ht[i].bucket[j].key_data) {
                    print_dbg_msg("\tht[%zu][%zu]\n", i, j);
                    print_dbg_msg("\t\tfree key\n");
                    free(ht_obj->ht[i].bucket[j].key_data);
                }
                if (ht_obj->ht[i].bucket[j].val_data) {
                    print_dbg_msg("\t\tfree val\n");
                    free(ht_obj->ht[i].bucket[j].val_data);
                }
            }
            free(ht_obj->ht[i].bucket);
        }
    }
    free(ht_obj->ht);
    free(ht_obj);
//#undef ht_obj
}

HTObject* HASH_TABLE_LIB_ResizeHashTable(HTObject **ht_obj_p, int new_cap) {
#define ht_obj (*ht_obj_p)
    if (!ht_obj) return NULL;
    HTObject *ht_obj_tmp = HASH_TABLE_LIB_NewHashTableObject(new_cap);
    if (!ht_obj_tmp) return NULL;
    for (size_t i = 0; i < ht_obj->ht_cap; ++i) {
        if (!ht_obj->ht[i].bucket) continue;
        for (size_t j = 0; j < ht_obj->ht[i].bucket_size; j++)
            HASH_TABLE_LIB_InsertHashTableEntry(ht_obj_tmp, &ht_obj->ht[i].bucket[j]);
    }
    HASH_TABLE_LIB_DestroyHashTableObject(ht_obj); ht_obj = ht_obj_tmp;
    return ht_obj;
#undef ht_obj
}


int HASH_TABLE_LIB_InsertHashTableEntry(HTObject *ht_obj, const HashTableEntry *ht_entry) {
//#define ht_obj (*ht_obj_p)
    print_dbg_msg("HASH_TABLE_LIB_InsertHashTableEntry\n");
    if (!ht_obj) return -1;
    if (ht_obj->ht_size >= ht_obj->ht_cap) {
        print_err_msg("Error: HashTable full!\n");
        return -1;
    }
    print_dbg_msg("after resize\n");

    int h_idx = ht_entry->key_hash % ht_obj->ht_cap;
    if (ht_obj->ht[h_idx].bucket_size >= ht_obj->ht[h_idx].bucket_cap) {
        size_t bucket_cap_new = ht_obj->ht[h_idx].bucket_cap * 2;
        HashTableEntry *bucket_new_p = (HashTableEntry*)realloc(ht_obj->ht[h_idx].bucket, bucket_cap_new * sizeof(HashTableEntry));
        if (!bucket_new_p) {
            print_err_msg("(HashTableEntry*)realloc(ht_obj->ht[h_idx].bucket, bucket_cap_new * sizeof(HashTableEntry)) failed\n");
            return -1;
        }
        ht_obj->ht[h_idx].bucket = bucket_new_p;
        ht_obj->ht[h_idx].bucket_cap = bucket_cap_new;
    }
    print_err_msg("before insert\n");
    ht_obj->ht[h_idx].bucket[ht_obj->ht[h_idx].bucket_size].key_len = ht_entry->key_len;
    ht_obj->ht[h_idx].bucket[ht_obj->ht[h_idx].bucket_size].key_hash = ht_entry->key_hash;
    ht_obj->ht[h_idx].bucket[ht_obj->ht[h_idx].bucket_size].val_len = ht_entry->val_len;


    ht_obj->ht[h_idx].bucket[ht_obj->ht[h_idx].bucket_size].key_data = (ubyte_t*)malloc(ht_entry->key_len);
    if (!ht_obj->ht[h_idx].bucket[ht_obj->ht[h_idx].bucket_size].key_data) {
        print_err_msg("(ubyte_t*)malloc(ht_entry->key_len) failed\n");
        return -1;
    }
    ht_obj->ht[h_idx].bucket[ht_obj->ht[h_idx].bucket_size].val_data = (ubyte_t*)malloc(ht_entry->val_len);
    if (!ht_obj->ht[h_idx].bucket[ht_obj->ht[h_idx].bucket_size].val_data) {
        print_err_msg("(ubyte_t*)malloc(ht_entry->val_len) failed\n");
        free(ht_obj->ht[h_idx].bucket[ht_obj->ht[h_idx].bucket_size].key_data);
        ht_obj->ht[h_idx].bucket[ht_obj->ht[h_idx].bucket_size].key_data = NULL;
        return -1;
    }
    memcpy(
        ht_obj->ht[h_idx].bucket[ht_obj->ht[h_idx].bucket_size].key_data,
        ht_entry->key_data,
        ht_entry->key_len
    );
    memcpy(
        ht_obj->ht[h_idx].bucket[ht_obj->ht[h_idx].bucket_size].val_data,
        ht_entry->val_data,
        ht_entry->val_len
    );

    ++ht_obj->ht[h_idx].bucket_size;
    ++ht_obj->ht_size;
    print_dbg_msg("before ret\n");
    return h_idx;
//#undef ht_obj
}


DataBuffer *HASH_TABLE_LIB_GetHashTableEntry(HTObject *ht_obj, void *key, size_t key_len) {
    if (!ht_obj) return NULL;
    DataBuffer *val = (DataBuffer*)malloc(sizeof(DataBuffer));
    if (!val) {
        print_err_msg("(DataBuffer*)malloc(sizeof(DataBuffer)) failed\n");
        return NULL;
    }

    int h_idx = fnv_1a_hash(key, key_len) % ht_obj->ht_cap;
    for (size_t i = 0; i < ht_obj->ht[h_idx].bucket_size; ++i) {
        if (ht_obj->ht[h_idx].bucket[i].key_data &&
            memcmp(ht_obj->ht[h_idx].bucket[i].key_data, key, key_len) == 0 &&
            ht_obj->ht[h_idx].bucket[i].key_len == key_len
        ) {
            val->data_len = ht_obj->ht[h_idx].bucket[i].val_len;
            val->data = (ubyte_t*)malloc(val->data_len);
            if (!val->data) {
                print_err_msg("(ubyte_t*)malloc(val->data_len) failed\n");
                free(val); val = NULL;
                return NULL;
            }
            memcpy(val->data, ht_obj->ht[h_idx].bucket[i].val_data, val->data_len);
            return val;
        }
    }
    return NULL;
}

void HASH_TABLE_LIB_DestroyDataBuffer(DataBuffer *buf) {
    if (!buf) return;
    if (buf->data) {
        free(buf->data); buf->data = NULL;
    }
    free(buf); //buf = NULL;
}

int HASH_TABLE_LIB_DeleteHashTableEntry(HTObject *ht_obj, void *key, size_t key_len) {
    if (!ht_obj) return -1;

    int h_idx = fnv_1a_hash(key, key_len) % ht_obj->ht_cap;

    for (size_t i = 0; i < ht_obj->ht[h_idx].bucket_size; ++i) {
        if (
            memcmp(ht_obj->ht[h_idx].bucket[i].key_data, key, key_len) == 0 &&
            ht_obj->ht[h_idx].bucket[i].key_len == key_len
        ) {
            if (ht_obj->ht[h_idx].bucket[i].key_data) {
                free(ht_obj->ht[h_idx].bucket[i].key_data);
                ht_obj->ht[h_idx].bucket[i].key_data = NULL;
            }
            if (ht_obj->ht[h_idx].bucket[i].val_data) {
                free(ht_obj->ht[h_idx].bucket[i].val_data);
                ht_obj->ht[h_idx].bucket[i].val_data = NULL;
            }
            if (i < ht_obj->ht[h_idx].bucket_size - 1) {
                memmove(
                    &ht_obj->ht[h_idx].bucket[i],
                    &ht_obj->ht[h_idx].bucket[i + 1],
                    (ht_obj->ht[h_idx].bucket_size - (i + 1)) * HT_ENTRY_SIZE // sizeof(struct _ht_entry)
                );
                memset(&ht_obj->ht[h_idx].bucket[ht_obj->ht[h_idx].bucket_size - 1], 0, sizeof(HashTableEntry));
            }
            --ht_obj->ht[h_idx].bucket_size;
            --ht_obj->ht_size;
            return h_idx;
        }
    }

    return -1;
}

