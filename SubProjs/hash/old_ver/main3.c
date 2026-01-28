#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_TABLE_SIZE 4096
#define MAX_ENTRY_COUNT HASH_TABLE_SIZE
#define MAX_KEY_SIZE    256

enum {
    UNUSED  = 0,
    USED    = 1,
    DELETED = 2
};

typedef char i8;
typedef unsigned long long hash_t;

typedef struct {
    char* key;
    char* val;
    i8    used;  // 0 = UNUSED, 1 = occupied
} HashEntry;

HashEntry hash_table[HASH_TABLE_SIZE] = {0};

static inline int find_str_nul(const char* str, int size) {
    int i = 0;
    while (i < size) {
        if (str[i] == '\0') {
            return i;
        }
        i++;
    }
    return -1;
}

hash_t hash(const char* str) {
#define FNV_OFFSET_BASIS 14695981039346656037ULL
#define FNV_PRIME        1099511628211ULL
    if (find_str_nul(str, MAX_KEY_SIZE) < 0) return 0;
    unsigned long long h = FNV_OFFSET_BASIS;
    size_t i = 0;
    while (str[i] != '\0') {
        h ^= (unsigned char)str[i];
        h *= FNV_PRIME;
        i++;
    }
    return h;
#undef FNV_OFFSET_BASIS
#undef FNV_PRIME
}

int insert_entry(const char* key, const char* val) {
    if (find_str_nul(key, MAX_KEY_SIZE) < 0) return -1;

    hash_t h = (hash_t)hash(key);
    size_t idx = h % HASH_TABLE_SIZE;

    while (hash_table[idx].used != UNUSED || hash_table[idx].used != DELETED) {
        if (strcmp(hash_table[idx].key, key) == 0) {
            // key already exists, overwriting val
            if (hash_table[idx].val) free(hash_table[idx].val);
            hash_table[idx].val = (char*)malloc(strlen(val) + 1);
            if (!hash_table[idx].val) {
                perror("malloc");
                return -1;
            }
            strcpy(hash_table[idx].val, val);
            return idx;
        }
        idx = (idx + 1) % HASH_TABLE_SIZE;  // go to next slot
    }

    hash_table[idx].key = (char*)malloc(strlen(key) + 1);
    hash_table[idx].val = (char*)malloc(strlen(val) + 1);
    if (!hash_table[idx].key || !hash_table[idx].val) {
        perror("malloc");
        return -1;
    }

    strcpy(hash_table[idx].key, key);
    strcpy(hash_table[idx].val, val);
    hash_table[idx].used = USED;
    return idx;
}

char* search_key(const char* key) {
    if (find_str_nul(key, MAX_KEY_SIZE) < 0) return NULL;
    hash_t h = hash(key);
    size_t idx = h % HASH_TABLE_SIZE;
    size_t start = idx;

    while (hash_table[idx].used != UNUSED || hash_table[idx].used != DELETED) {
        if (strcmp(hash_table[idx].key, key) == 0) {
            return hash_table[idx].val; // found
        }
        idx = (idx + 1) % HASH_TABLE_SIZE;   // linear probing
        if (idx == start) break;        // full loop → not found
    }
    return NULL;  // key not found
}

int delete_entry(const char* key) {
    if (find_str_nul(key, MAX_KEY_SIZE) < 0) return -1;

    hash_t h = (hash_t)hash(key);
    size_t idx = h % HASH_TABLE_SIZE;

    while (hash_table[idx].used != UNUSED || hash_table[idx].used != DELETED) {
        if (strcmp(hash_table[idx].key, key) == 0) {
            if (hash_table[idx].key) free(hash_table[idx].key);
            if (hash_table[idx].val) free(hash_table[idx].val);
            hash_table[idx].used = DELETED;
            return idx;
        }
        idx = (idx + 1) % HASH_TABLE_SIZE;  // go to next slot
    }
    return idx;
}

void reset_hash_table(void) {
    size_t i = 0;
    while (i < MAX_ENTRY_COUNT) {
        if (hash_table[i].key) free(hash_table[i].key);
        if (hash_table[i].val) free(hash_table[i].val);
        hash_table[i].used = UNUSED;
        i++;
    }
}

int main() {
    insert_entry("User01.FirstName", "Alice");
    insert_entry("User01.LastName", "Smith");
    insert_entry("User01.Age", "27");
    insert_entry("User01.Country", "Canada");
    insert_entry("User01.City", "Toronto");
    insert_entry("User01.Email", "alice.smith@example.com");
    insert_entry("User01.Username", "A1iceS");
    insert_entry("User01.Language", "English");
    insert_entry("User01.Occupation", "Engineer");
    insert_entry("User01.Hobby", "Chess");
    printf("%s\n", search_key("User01.FirstName"));
    delete_entry("User01.FirstName");
    printf("%s\n", search_key("User01.Occupation"));

    return 0;
}