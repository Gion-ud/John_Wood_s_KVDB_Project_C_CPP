#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <stdint.h>

#define printerrf(...) fprintf(stderr, __VA_ARGS__)

#define MAX_ENTRY_COUNT 4096u
#define HASH_TABLE_SIZE MAX_ENTRY_COUNT

typedef struct {
    unsigned char* data;
    unsigned int size;
} KV;
typedef struct {
    KV key;
    KV val;
} KVPair;



#define FNV_OFFSET_BASIS    14695981039346656037ULL
#define FNV_PRIME           1099511628211ULL
#define hash_str(key) hash(key, strlen(key) + 1)
unsigned long long hash(const unsigned char* key, size_t len) {
    unsigned long long hash_val = FNV_OFFSET_BASIS;
    for (size_t i = 0; i < len; i++) {
        hash_val ^= key[i];
        hash_val *= FNV_PRIME;
    }
    return hash_val;
}
#undef HASH_INIT_NUM
#undef HASH_MUL_NUM
#define search(key_str, key) find_key(key_str, key, &hash_table, strlen(key_str) + 1)
int find_key(const unsigned char* key, KVPair* kv_arr, unsigned long long (*hash_table)[HASH_TABLE_SIZE], size_t len) {
#define key_hash hash_str(key)
    unsigned long long idx = key_hash % HASH_TABLE_SIZE; // hash table index
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        if (idx == (*hash_table)[i] % HASH_TABLE_SIZE) {
            printerrf("hash_idx=%llu;idx=%d\n", key_hash % HASH_TABLE_SIZE, i);
            if (memcmp(key, kv_arr[i].key.data, len) != 0) {
                printerrf("No match\n");
            } else {
                printerrf("Matching key found: \"%s\"\n", kv_arr[i].key.data);
                return i; // key val entry index
            }
        }
    }
    return -1;
#undef key_hash
}

// #define BUFFER_SIZE 4096u

static size_t entry_count = 0;
static inline void insert_str_kv(KVPair* kv, const char* key_str, const char* val_str) {
    kv[entry_count].key.data = (unsigned char*)malloc(strlen(key_str) + 1);
    if (!kv[entry_count].key.data) {
        perror("malloc");
        exit(1);
    }
    kv[entry_count].val.data = (unsigned char*)malloc(strlen(val_str) + 1);
    if (!kv[entry_count].val.data) {
        perror("malloc");
        exit(1);
    }
    strcpy(kv[entry_count].key.data, key_str);
    kv[entry_count].key.size = strlen(kv[entry_count].key.data) + 1;
    strcpy(kv[entry_count].val.data, val_str);
    kv[entry_count].val.size = strlen(kv[entry_count].val.data) + 1;

    entry_count++;
}

#define BEGIN_PRINT_KV_PAIR \
    fputs("+--------------------------------+--------------------------------+\n", stdout);\
    printf("|%-32s|%-32s|\n", "key", "val");\
    fputs("+--------------------------------+--------------------------------+\n", stdout);
#define END_PRINT_KV_PAIR \
    fputs("+--------------------------------+--------------------------------+\n", stdout);\
    putchar('\n');

//static inline void print_kv_pair(KV* key, KV* val, int idx) {\
    //printf("\"%s\"=\"%s\"\n", key[idx].data, val[idx].data);\
    printf("key=\"%s\";val=\"%s\"\n", key[idx].data, val[idx].data);\
}
static inline void print_all_kv_pair(KVPair* kv) {
    BEGIN_PRINT_KV_PAIR
    for (size_t i = 0; i < entry_count; i++) {
        printf("|%-32s|%-32s|\n", kv[i].key.data, kv[i].val.data);
    }
    END_PRINT_KV_PAIR
}
 

int main() {
    KVPair kv[MAX_ENTRY_COUNT] = {0};
    //KV key[MAX_ENTRY_COUNT] = {0}, val[MAX_ENTRY_COUNT] = {0};

    //insert_str_kv(key, val, "User01.FirstName", "Alex");
    insert_str_kv(kv, "User01.FirstName", "Alex");
    insert_str_kv(kv, "User01.LastName", "Young");
    insert_str_kv(kv, "User01.DateOfBirth", "16/05/2001");
    insert_str_kv(kv, "User01.Ethnicity", "British");
    insert_str_kv(kv, "User01.CountryOfOrigin", "United Kingdom");
    insert_str_kv(kv, "User01.Country", "New Zealand");
    insert_str_kv(kv, "User01.City", "Auckland");
    insert_str_kv(kv, "User01.UserName", "usNNwnn_dhc");

    print_all_kv_pair(kv);

    unsigned long long hash_table[HASH_TABLE_SIZE] = {0};
    for (size_t i = 0; i < entry_count; i++) {
        hash_table[i] = hash(kv[i].key.data, kv[i].key.size);
        printf("hash_idx[%d]=%llu\n", i, hash_table[i] % HASH_TABLE_SIZE);
    }
    putchar('\n');
    int idx = search("User01.Country", kv);
    printf("User01.Country=`%s`\n", kv[idx].val.data);
    for (unsigned int i = 0; i < MAX_ENTRY_COUNT; i++) {
        if (kv[i].key.data) free(kv[i].key.data);
        if (kv[i].val.data) free(kv[i].val.data);
    }
    printerrf("done\n");

    return 0;
}