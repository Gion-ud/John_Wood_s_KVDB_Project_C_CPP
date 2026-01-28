#include "../include/hash_table_lib.h"

hash_t hash(const byte_t* key, keysize_t size) {
#define FNV_OFFSET_BASIS 14695981039346656037ULL
#define FNV_PRIME        1099511628211ULL
    hash_t h = FNV_OFFSET_BASIS;
    keysize_t i = 0;
    while (i < size) {
        h ^= (byte_t)key[i];
        h *= FNV_PRIME;
        i++;
    }
    return h;
#undef FNV_OFFSET_BASIS
#undef FNV_PRIME
}

int InitHashTableObject(HashTableObject* pObj, int MaxEntryCount) {
    if (MaxEntryCount <= 0) return INIT_HTOBJ_FAILED;
    // if (pObj) { print_err_msg("HashTableObject already initialised\n"); return; }
    pObj->entry_count = 0;
    pObj->nonempty_entry_count = 0;
    pObj->hash_table_size = MaxEntryCount;
    pObj->hash_entry_arr = (HashEntry*)calloc(MaxEntryCount, sizeof(HashEntry));

    if (!pObj->hash_entry_arr) {
        print_err_msg("(HashEntry*)calloc(MaxEntryCount, sizeof(HashEntry)) failed\n");
        return INIT_HTOBJ_FAILED;
    }
    print_dbg_msg("[DEBUG] Initialised %d counts of HashEntry in HashTable\n", MaxEntryCount);
    return MaxEntryCount;
}

void DestroyHashTableObject(HashTableObject* pObj) {
    if (!pObj) return;
    for (size_t i = 0; i < pObj->hash_table_size; i++) {
        if (pObj->hash_entry_arr[i].key) { free(pObj->hash_entry_arr[i].key); pObj->hash_entry_arr[i].key = NULL; };
        pObj->hash_entry_arr[i].key_size = 0;
        pObj->hash_entry_arr[i].key_hash = 0;
    }
    if (pObj->hash_entry_arr) {
        free(pObj->hash_entry_arr);
        print_dbg_msg("HashTableObject got destroyed at address 0x%" PRIptr "\n", (uqword_t)&pObj);
    }
    // free(pObj);
}

hidx_t insert_entry(HashTableObject* pObj, const byte_t* key, keysize_t key_size) {
#define HashTable   pObj->hash_entry_arr
#define table_size  pObj->hash_table_size
    //print_dbg_msg("[DEBUG] hidx_t insert_entry(HashTableObject* pObj, const byte_t* key, keysize_t key_size);\n");
    if (!key || !HashTable || key_size > MAX_KEY_SIZE) {
        print_err_msg("[ERROR] Failed to insert entry\n");
        return INSERT_ENTRY_FAILED;
    }
    if (pObj->entry_count == pObj->hash_table_size) {
        print_err_msg("[ERROR] Hash table full\n");
        return HASH_TABLE_FULL;
    }
    hash_t h = hash(key, key_size);
    hidx_t hash_idx = h % table_size;
    hidx_t start_idx = hash_idx;
    while (LOOP_RUNNING) {
        if (HashTable[hash_idx].flag == ENTRY_EMPTY || HashTable[hash_idx].flag == ENTRY_DELETED) {
            HashTable[hash_idx].key_hash = h;
            HashTable[hash_idx].key_size = key_size;
            HashTable[hash_idx].key = (byte_t*)malloc(key_size * sizeof(byte_t));
            if (!HashTable[hash_idx].key) {
                print_err_msg("(byte_t)malloc(key_size * sizeof(byte_t)) failed\n");
                HashTable[hash_idx].key_size = 0;
                HashTable[hash_idx].key_hash = 0;
                HashTable[hash_idx].flag = ENTRY_EMPTY;
                print_err_msg("[ERROR] Failed to insert entry\n");
                return INSERT_ENTRY_FAILED;
            }
            if (HashTable[hash_idx].flag == ENTRY_EMPTY) pObj->nonempty_entry_count++;
            (pObj->entry_count)++;
            HashTable[hash_idx].flag = ENTRY_USED;
            memcpy(HashTable[hash_idx].key, key, HashTable[hash_idx].key_size);
            print_dbg_msg("[DEBUG] successfully inserted key at index %d\n", hash_idx);
            return hash_idx;
        } else {
            hash_idx = (hash_idx + 1) % table_size;
            if (hash_idx == start_idx) {
                print_err_msg("[ERROR] Failed to insert entry\n");
                return INSERT_ENTRY_FAILED;
            } else continue;
        }
    }
#undef HashTable
#undef table_size
};

hidx_t search_key(HashTableObject* pObj, const byte_t* key, keysize_t key_size) {
#define HashTable   pObj->hash_entry_arr
#define table_size  pObj->hash_table_size
    if (!key || !HashTable || key_size > MAX_KEY_SIZE) {
        print_err_msg("[ERROR] Failed to search key\n");
        return SEARCH_KEY_FAILED;
    }
    hash_t h = hash(key, key_size);
    hidx_t hash_idx = h % table_size;
    hidx_t start_idx = hash_idx;
    while (HashTable[hash_idx].flag != ENTRY_EMPTY) {
        if (HashTable[hash_idx].flag == ENTRY_DELETED) goto search_next_key;
        if (HashTable[hash_idx].key_size == key_size && memcmp(key, HashTable[hash_idx].key, key_size) == 0) {
            print_dbg_msg("[DEBUG] Key found at index %d\n", hash_idx);
            return hash_idx;
        } else {
        search_next_key:
            hash_idx = (hash_idx + 1) % table_size;
            if (hash_idx == start_idx) {
                print_err_msg("[ERROR] KEY_NOT_FOUND\n");
                return KEY_NOT_FOUND;
            } else continue;
        }
    }
    return KEY_NOT_FOUND;
#undef HashTable
#undef table_size
};

hidx_t delete_entry(HashTableObject* pObj, const byte_t* key, keysize_t key_size) {
#define HashTable   pObj->hash_entry_arr
#define table_size  pObj->hash_table_size
    if (!key || !HashTable || key_size > MAX_KEY_SIZE) {
        print_err_msg("[ERROR] DEL_KEY_FAILED\n");
        return DEL_KEY_FAILED;
    }
    hash_t h = hash(key, key_size);

    hidx_t hash_idx = h % table_size;
    hidx_t start_idx = hash_idx;
    while (HashTable[hash_idx].flag != ENTRY_EMPTY) {
        if (!HashTable[hash_idx].key) goto search_next_key;
        if (HashTable[hash_idx].key_size == key_size && memcmp(key, HashTable[hash_idx].key, key_size) == 0) {
            HashTable[hash_idx].key_hash = 0;
            HashTable[hash_idx].key_size = 0;
            if (HashTable[hash_idx].key) free(HashTable[hash_idx].key);
            HashTable[hash_idx].key = NULL;
            HashTable[hash_idx].flag = ENTRY_DELETED;
            pObj->entry_count--;
            print_dbg_msg("[DEBUG] Key deleted at index %d\n", hash_idx);
            return DEL_KEY_SUCCESS;
            // return hash_idx;
        } else {
        search_next_key:
            hash_idx = (hash_idx + 1) % table_size;
            if (hash_idx == start_idx) {
                print_err_msg("[ERROR] KEY_NOT_FOUND\n");
                return KEY_NOT_FOUND;
            } else continue;
        }
    }
    return KEY_NOT_FOUND;
#undef HashTable
#undef table_size
};
