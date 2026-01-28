#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define print_err_msg(...) fprintf(stderr, __VA_ARGS__)
#define print_dbg_msg(...) fprintf(stderr, __VA_ARGS__)
#define LOOP_RUNNING 1

typedef unsigned long long hash_t;
typedef long hidx_t;
typedef size_t keysize_t;
typedef unsigned char byte_t;

typedef unsigned long long ptr_t;
#define PRIptr ".16llx"


#define MAX_KEY_SIZE 256

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

typedef struct HashEntry HashEntry;
struct HashEntry {
    byte_t*     key;
    keysize_t   key_size;
    hash_t      hash;
};

#define MAX_ENTRY_COUNT 16
#define HASH_TABLE_SIZE MAX_ENTRY_COUNT

HashEntry* InitHashTable(size_t MaxEntryCount) {
    HashEntry* HashTable = (HashEntry*)calloc(MaxEntryCount, sizeof(HashEntry));
    if (!HashTable) {
        print_err_msg("(HashEntry*)calloc(MaxEntryCount, sizeof(HashEntry)) failed\n");
        return NULL;
    }
    print_dbg_msg("[DEBUG] Initialised %zu counts of HashEntry in HashTable\n", MaxEntryCount);
    return HashTable;
}
void DestroyHashTable(HashEntry* HashTable, size_t table_size) {
    for (size_t i = 0; i < table_size; i++) {
        if (HashTable[i].key) { free(HashTable[i].key); HashTable[i].key = NULL; };
        HashTable[i].key_size = 0;
        HashTable[i].hash = 0;
    }
    if (HashTable) {
        free(HashTable);
        print_dbg_msg("HashTable got destroyed at address 0x%" PRIptr, (ptr_t)&HashTable);
    }
}

#define INSERT_ENTRY_FAILED -1
hidx_t insert_entry(HashEntry* HashTable, size_t table_size, const byte_t* key, keysize_t key_size) {
    if (!key || !HashTable || key_size > MAX_KEY_SIZE) {
        print_err_msg("[ERROR] Failed to insert entry\n");
        return INSERT_ENTRY_FAILED;
    }
    hash_t h = hash(key, key_size);

    hidx_t hash_idx = h % table_size;
    hidx_t start_idx = hash_idx;
    while (LOOP_RUNNING) {
        if ((!HashTable[hash_idx].hash) && (!HashTable[hash_idx].key) && (!HashTable[hash_idx].key_size)) {
            HashTable[hash_idx].hash = h;
            HashTable[hash_idx].key_size = key_size;
            HashTable[hash_idx].key = (byte_t*)malloc(key_size * sizeof(byte_t));
            if (!HashTable[hash_idx].key) {
                print_err_msg("(byte_t)malloc(key_size * sizeof(byte_t)) failed\n");
                HashTable[hash_idx].key_size = 0;
                HashTable[hash_idx].hash = 0;
                print_err_msg("[ERROR] Failed to insert entry\n");
                return INSERT_ENTRY_FAILED;
            }
            memcpy(HashTable[hash_idx].key, key, HashTable[hash_idx].key_size);
            print_dbg_msg("[DEBUG] successfully inserted key at index %ld\n", hash_idx);
            return hash_idx;
        } else {
            hash_idx = (hash_idx + 1) % table_size;
            if (hash_idx == start_idx) {
                print_err_msg("[ERROR] Failed to insert entry\n");
                return INSERT_ENTRY_FAILED;
            }
        }
    }
};

#define SEARCH_KEY_FAILED -2
#define KEY_NOT_FOUND -1
hidx_t search_key(HashEntry* HashTable, size_t table_size, const byte_t* key, keysize_t key_size) {
    if (!key || !HashTable || key_size > MAX_KEY_SIZE) {
        print_err_msg("[ERROR] Failed to search key\n");
        return SEARCH_KEY_FAILED;
    }
    hash_t h = hash(key, key_size);

    hidx_t hash_idx = h % table_size;
    hidx_t start_idx = hash_idx;
    while (LOOP_RUNNING) {
        if ((!HashTable[hash_idx].hash) || (!HashTable[hash_idx].key) || (!HashTable[hash_idx].key_size)) {
            goto search_next_key;
        }
        if (HashTable[hash_idx].key_size == key_size || memcmp(key, HashTable[hash_idx].key, key_size) == 0) {
            print_dbg_msg("[DEBUG] Key found at index %ld\n", hash_idx);
            return hash_idx;
        } else {
            goto search_next_key;
        }
    search_next_key:
        hash_idx = (hash_idx + 1) % table_size;
        if (hash_idx == start_idx) {
            print_err_msg("[ERROR] Key not found\n");
            return KEY_NOT_FOUND;
        } else {
            continue;
        }
    }
};

#define DEL_KEY_FAILED -2
#define DEL_KEY_SUCCESS 0
hidx_t delete_entry(HashEntry* HashTable, size_t table_size, const byte_t* key, keysize_t key_size) {
    if (!key || !HashTable || key_size > MAX_KEY_SIZE) {
        print_err_msg("[ERROR] DEL_KEY_FAILED\n");
        return DEL_KEY_FAILED;
    }
    hash_t h = hash(key, key_size);

    hidx_t hash_idx = h % table_size;
    hidx_t start_idx = hash_idx;
    while (LOOP_RUNNING) {
        if ((!HashTable[hash_idx].hash) || (!HashTable[hash_idx].key) || (!HashTable[hash_idx].key_size)) {
            goto search_next_key;
        }
        if (memcmp(key, HashTable[hash_idx].key, key_size) == 0) {
            HashTable[hash_idx].hash = 0;
            HashTable[hash_idx].key_size = 0;
            free(HashTable[hash_idx].key);
            HashTable[hash_idx].key = NULL;
            print_dbg_msg("[DEBUG] Key deleted at index %ld\n", hash_idx);
            return DEL_KEY_SUCCESS;
            // return hash_idx;
        } else {
            goto search_next_key;
        }
    search_next_key:
        hash_idx = (hash_idx + 1) % table_size;
        if (hash_idx == start_idx) {
            print_err_msg("[ERROR] Key not found\n");
            return KEY_NOT_FOUND;
        } else {
            continue;
        }
    }
};

int main() {
    HashEntry* HashTable = InitHashTable(HASH_TABLE_SIZE);
    if (!HashTable) return 1;
    //hidx_t ret;
    insert_entry(HashTable, HASH_TABLE_SIZE, "this_is_a_key", strlen("this_is_a_key") + 1);
    insert_entry(HashTable, HASH_TABLE_SIZE, "user01.username", strlen("user01.username") + 1);
    insert_entry(HashTable, HASH_TABLE_SIZE, "user01.password", strlen("user01.password") + 1);
    insert_entry(HashTable, HASH_TABLE_SIZE, "user01.email", strlen("user01.email") + 1);
    insert_entry(HashTable, HASH_TABLE_SIZE, "user01.phone_number", strlen("user01.phone_number") + 1);

    search_key(HashTable, HASH_TABLE_SIZE, "user01.phone_number", strlen("user01.phone_number") + 1);



    DestroyHashTable(HashTable, HASH_TABLE_SIZE);
    return 0;
}