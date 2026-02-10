#include "hash_index_lib.h"

HTObject *HASH_INDEX_LIB_HTObject_create(int ht_cap) {
    HTObject *ht_obj = (HTObject*)malloc(sizeof(HTObject));
    if (!ht_obj) {
        print_err_msg("(HTObject*)malloc(sizeof(HTObject)) failed\n");
        return NULL;
    }

    ht_obj->ht_cap = ht_cap;
    ht_obj->ht_entry_cnt = 0;
    ht_obj->ht_del_cnt = 0;
    ht_obj->ht = (HashTableSlot*)calloc(ht_cap, sizeof(HashTableSlot));

    if (!ht_obj->ht) {
        print_err_msg("(HashTableEntry*)calloc(ht_cap, sizeof(HashTableEntry)) failed\n");
        goto NewHashTableObject_failed_cleanup;
    }

    for (int i = 0; i < ht_cap; ++i) {
        ht_obj->ht[i].bucket_cap = INIT_BUCKET_CAP;
        ht_obj->ht[i].bucket_size = 0;
        ht_obj->ht[i].bucket = (HashTableEntry*)calloc(INIT_BUCKET_CAP, sizeof(HashTableEntry));
        if (!ht_obj->ht[i].bucket) {
            print_err_msg("(HashTableEntry*)calloc(INIT_BUCKET_CAP, sizeof(HashTableEntry)) failed\n");
            goto NewHashTableObject_failed_cleanup;
        }
    }

    return ht_obj;
NewHashTableObject_failed_cleanup:

    if (!ht_obj) return NULL;
    if (ht_obj->ht) {
        for (size_t i = 0; i < ht_obj->ht_cap; ++i) {
            if (ht_obj->ht[i].bucket) free(ht_obj->ht[i].bucket);
        }
        free(ht_obj->ht);
    }
    return NULL;
}

void HASH_INDEX_LIB_HTObject_destroy(HTObject *ht_obj) {
    if (!ht_obj) return;
    if (!ht_obj->ht) { free(ht_obj); return; }
    for (size_t i = 0; i < ht_obj->ht_cap; ++i) {
        if (ht_obj->ht[i].bucket) free(ht_obj->ht[i].bucket);
    }
    free(ht_obj->ht);
    free(ht_obj);
}

HTObject* HASH_INDEX_LIB_HTObject_resize(HTObject *ht_obj, int new_cap) {
    if (!ht_obj) return NULL;
    HTObject *ht_obj_new = HASH_INDEX_LIB_HTObject_create(new_cap);
    if (!ht_obj_new) return NULL;
    int idx = -1;
    for (size_t i = 0; i < ht_obj->ht_cap; ++i) {
        if (!ht_obj->ht[i].bucket) continue;
        for (size_t j = 0; j < ht_obj->ht[i].bucket_size; j++) {
            if (ht_obj->ht[i].bucket[j].state == HT_ENTRY_INUSE) {
                idx = HASH_INDEX_LIB_HTObject_insert(
                    ht_obj_new,
                    ht_obj->ht[i].bucket[j].key_hash,
                    ht_obj->ht[i].bucket[j].entry_id
                );
                if (idx < 0) {
                    print_err_msg(
                        "HASH_INDEX_LIB_HTObject_insert: failed to insert entry [%zu, %zu]\n",
                        i, j
                    );
                    continue;
                };
            }
        }
    }
    HASH_INDEX_LIB_HTObject_destroy(ht_obj);
    return ht_obj_new;
}

static inline int HASH_INDEX_LIB_HTObject_resize_bucket(HashTableSlot *ht_slot, size_t new_size) {
    HashTableEntry *bucket_new_p = (HashTableEntry*)realloc(ht_slot->bucket, new_size * sizeof(HashTableEntry));
    if (!bucket_new_p) {
        perror("(HashTableEntry*)realloc(ht_slot->bucket, new_size * sizeof(HashTableEntry)) failed");
        return -1;
    }
    ht_slot->bucket_cap = new_size;
    ht_slot->bucket = bucket_new_p;
    return new_size;
}

int HASH_INDEX_LIB_HTObject_insert(HTObject *ht_obj, hash_t key_hash, ulong_t entry_id) {
    if (!ht_obj) return -1;
    if (ht_obj->ht_entry_cnt >= ht_obj->ht_cap) {
        print_err_msg("Error: HashTable full!\n");
        return -1;
    }

    int h_idx = key_hash % ht_obj->ht_cap;
    if (ht_obj->ht[h_idx].bucket_size >= ht_obj->ht[h_idx].bucket_cap) {
        int ret = HASH_INDEX_LIB_HTObject_resize_bucket(&ht_obj->ht[h_idx], ht_obj->ht[h_idx].bucket_cap * 2);
        if (ret < 0) return -1;
    }

#define bucket_idx ht_obj->ht[h_idx].bucket_size
    ht_obj->ht[h_idx].bucket[bucket_idx].entry_id = entry_id;
    ht_obj->ht[h_idx].bucket[bucket_idx].key_hash = key_hash;
    ht_obj->ht[h_idx].bucket[bucket_idx].state = (ubyte_t)HT_ENTRY_INUSE;

    ++bucket_idx;
    ++ht_obj->ht_slot_used_cnt;
    ++ht_obj->ht_entry_cnt;

    ht_obj->ht[h_idx].slot_state = HT_SLOT_INUSE;
#undef bucket_idx

    return h_idx;
}

int HASH_INDEX_LIB_HTObject_delete(HTObject *ht_obj, hash_t key_hash, ulong_t entry_id) {
    if (!ht_obj) return -1;

    int h_idx = key_hash % ht_obj->ht_cap;

    for (size_t i = 0; i < ht_obj->ht[h_idx].bucket_size; ++i) {
        if (
            ht_obj->ht[h_idx].bucket[i].entry_id == (int)entry_id
        ) {
            ht_obj->ht[h_idx].bucket[i].entry_id = -1;
            ht_obj->ht[h_idx].bucket[i].key_hash = 0;
            ht_obj->ht[h_idx].bucket[i].state = (ubyte_t)HT_ENTRY_DELETED;
            --ht_obj->ht_entry_cnt;
            ++ht_obj->ht_del_cnt;
            return h_idx;
        }
    }

    return -1;
}

HTObject *HASH_INDEX_LIB_HTObject_compact(HTObject *ht_obj) {
    if (!ht_obj) return NULL;
    HTObject *ht_obj_new = HASH_INDEX_LIB_HTObject_create(ht_obj->ht_cap);
    int ret = -1;
    for (size_t i = 0; i < ht_obj->ht_cap; ++i) {
        if (!ht_obj->ht[i].bucket) continue;
        for (size_t j = 0; j < ht_obj->ht[i].bucket_size; ++j) {
            if (ht_obj->ht[i].bucket[j].state != HT_ENTRY_INUSE) continue;
            ret = HASH_INDEX_LIB_HTObject_insert(
                ht_obj_new,
                ht_obj->ht[i].bucket[j].key_hash,
                ht_obj->ht[i].bucket[j].entry_id
            );
            if (ret < 0) {
                print_err_msg(
                    "HASH_INDEX_LIB_HTObject_compact: HASH_INDEX_LIB_HTObject_insert failed at entry (i=%zu, j=%zu)\n",
                    i, j
                );
                continue;
            }
        }
    }
    HASH_INDEX_LIB_HTObject_destroy(ht_obj);
    return ht_obj_new;
}
