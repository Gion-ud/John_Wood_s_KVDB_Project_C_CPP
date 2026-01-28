#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <stdint.h>

#define printerrf(...) fprintf(stderr, __VA_ARGS__)

#define MAX_ENTRY_COUNT 16
#define HASH_TABLE_SIZE MAX_ENTRY_COUNT
#define KV_ENTRY_SIZE sizeof(KVEntry)
//#define KV_DATA_ENTRY_TABLE_SIZE (MAX_ENTRY_COUNT * sizeof(KVEntry))

static const unsigned char Magic[4] = { 'K', 'V', '\n', '\0' };

typedef struct {
    unsigned char magic[4];
    unsigned int entry_count;
    unsigned int max_entry_count;
    unsigned index_entry_size;
    unsigned int kv_data_filepath_len;
} KVIdxHeader;

typedef struct {
    unsigned int key_size;
    unsigned int val_size;
    unsigned long long key_offset;
    unsigned long long val_offset;
} KVEntry;
typedef struct {
    unsigned char* data;
    unsigned int size;
} KV;

#define HASH_INIT_NUM 2166136261u
#define HASH_MUL_NUM 16777619u
#define hash_str(key) hash(key, strlen(key) + 1)
unsigned long long hash(const unsigned char* key, size_t len) {
    unsigned long long hash_val = HASH_INIT_NUM;
    for (size_t i = 0; i < len; i++) {
        hash_val ^= key[i];
        hash_val *= HASH_MUL_NUM;
    }
    return hash_val;
}
#undef HASH_INIT_NUM
#undef HASH_MUL_NUM
#define search(key_str, key) find_key(key_str, key, &hash_table, strlen(key_str) + 1)
int find_key(const unsigned char* key, KV* key_arr, unsigned long long (*hash_table)[HASH_TABLE_SIZE], size_t len) {
    unsigned long long key_hash = hash_str(key);
    for (int i = 0; i < HASH_TABLE_SIZE; i++) {
        if (key_hash % HASH_TABLE_SIZE == (*hash_table)[i] % HASH_TABLE_SIZE) {
            printf("hash_idx=%llu;idx=%d\n", key_hash % HASH_TABLE_SIZE, i);
            if (memcmp(key, key_arr[i].data, len) != 0) {
                printf("No match\n");
            } else {
                printf("Matching key found: \"%s\"\n", key_arr[i].data);
                return i;
            }
        }
    }
    return -1;
}

#define BUFFER_SIZE 4096u

typedef struct {
    unsigned char* start_ptr;
    unsigned long long mem_buf_cur;
    size_t mem_buffer_size;
} MemBuffer;

typedef struct {
    KVEntry*    data_entry_table;
    MemBuffer*  p_mem_buf;
    unsigned int entry_count;
} KVObject;

MemBuffer init_mem_buf(size_t buf_size) {
    MemBuffer mem_buf;
    mem_buf.mem_buf_cur = 0;
    mem_buf.mem_buffer_size = buf_size;
    mem_buf.start_ptr = (unsigned char*)calloc(buf_size, sizeof(unsigned char));
    if (!mem_buf.start_ptr) {
        perror("calloc");
        exit(1);
    }
    return mem_buf;
}
KVEntry* init_mem_kv_store(MemBuffer* p_mem_buf, size_t max_entry_count) {
    #define mem_buf (*p_mem_buf)
    KVEntry* data_entry_table = (KVEntry*)malloc(max_entry_count * sizeof(KVEntry));
    if (!data_entry_table) {
        perror("malloc");
        exit(1);
    }
    //memcpy(mem_buf.start_ptr + mem_buf.mem_buf_cur, (unsigned char*)Magic, sizeof(Magic));
    //mem_buf.mem_buf_cur += sizeof(Magic);

    // mem_buf.mem_buf_cur += max_entry_count * sizeof(KVEntry);
    // printf("mem_buf.mem_buf_cur=%llu\n", mem_buf.mem_buf_cur);
    // !!!MUST FREE!!!
    return data_entry_table;
    #undef mem_buf
}
void insert_mem_kv_entry(MemBuffer* p_mem_buf, KVEntry* data_entry_table, KV key, KV val) {
#define mem_buf (*p_mem_buf)
#define MEM_BUF_CUR mem_buf.mem_buf_cur
#define MEM_BUF_START mem_buf.start_ptr

    memcpy(MEM_BUF_START + MEM_BUF_CUR, key.data, key.size);
    data_entry_table->key_size = key.size;
    data_entry_table->key_offset = MEM_BUF_CUR;
    MEM_BUF_CUR += key.size;

    memcpy(MEM_BUF_START + MEM_BUF_CUR, val.data, val.size);
    data_entry_table->val_size = val.size;
    data_entry_table->val_offset = MEM_BUF_CUR;
    MEM_BUF_CUR += val.size;
    //printerrf(\
        "key.size=%u\n"\
        "key.offset=%llu\n"\
        "val.size=%u\n"\
        "val.offset=%llu\n",\
        data_entry_table->key_size,\
        data_entry_table->key_offset,\
        data_entry_table->val_size,\
        data_entry_table->val_offset\
    );

#undef MEM_BUF_CUR
#undef MEM_BUF_START
#undef mem_buf
}

void write_index_table(MemBuffer* p_mem_buf, KVEntry* data_entry_table, size_t max_entry_count) {
#define mem_buf (*p_mem_buf)
#define MEM_BUF_START mem_buf.start_ptr
    unsigned long long pos = sizeof(Magic);
    for (size_t i = 0; i < max_entry_count; i++) {
        memcpy(MEM_BUF_START + pos, &data_entry_table[i], KV_ENTRY_SIZE);
        pos += KV_ENTRY_SIZE;
    }

#undef MEM_BUF_CUR
#undef mem_buf
}

void destroy_mem_buf(MemBuffer* mem_buf) {
    if (mem_buf->start_ptr) {
        free(mem_buf->start_ptr);
    };
}
static inline void print_kv_pair(MemBuffer* p_mem_buf, KVEntry* data_entry_table, int idx) {
    #define mem_buf (*p_mem_buf)
    printf(
        "\"%s\"=\"%s\"\n",
        mem_buf.start_ptr + data_entry_table[idx].key_offset,
        mem_buf.start_ptr + data_entry_table[idx].val_offset
    );
    #undef mem_buf
}

static inline void set_str_kv(KV* key, KV* val, const char* key_str, const char* val_str) {
    strcpy((*key).data, key_str);
    (*key).size = strlen((*key).data) + 1;
    strcpy((*val).data, val_str);
    (*val).size = strlen((*val).data) + 1;
}

int main() {
    FILE* fp = fopen("kv_data.bin", "wb+");
    FILE* fpidx = fopen("index.dat", "wb+");
    FILE* fphash = fopen("key_hash.bin", "wb+");
    if (!fp || !fpidx || !fphash) {
        perror("fopen");
        exit(1);
    }
    MemBuffer mem_buf = init_mem_buf(BUFFER_SIZE);
    KVEntry* data_entry_table = (KVEntry*)init_mem_kv_store(&mem_buf, MAX_ENTRY_COUNT);

    KV key[MAX_ENTRY_COUNT], val[MAX_ENTRY_COUNT];
    for (unsigned int i = 0; i < MAX_ENTRY_COUNT; i++) {
        key[i].data = alloca(32);
        val[i].data = alloca(32);
    }

    set_str_kv(&key[0], &val[0], "User.FirstName", "John");
    set_str_kv(&key[1], &val[1], "User.LastName", "Wood");
    set_str_kv(&key[2], &val[2], "User.DateOfBirth", "04/04/2009");
    set_str_kv(&key[3], &val[3], "User.Ethnicity", "Chinese");
    set_str_kv(&key[4], &val[4], "User.CountryOfOrigin", "China");
    set_str_kv(&key[5], &val[5], "User.Country", "New Zealand");

    size_t entry_count = 6;

    for (size_t i = 0; i < entry_count; i++) {
        insert_mem_kv_entry(&mem_buf, &data_entry_table[i], key[i], val[i]);
        fputs("key=val:\t", stdout);print_kv_pair(&mem_buf, data_entry_table, i);
        //putchar('\n');
    }
    putchar('\n');

    fseek(fp, 0, SEEK_SET);
    fwrite(mem_buf.start_ptr, BUFFER_SIZE, 1, fp);
    destroy_mem_buf(&mem_buf);
    fclose(fp);

    //write_index_table(&mem_buf, data_entry_table, MAX_ENTRY_COUNT);
    fseek(fpidx, 0, SEEK_SET);
    {
        KVIdxHeader header = {0};
        memcpy(header.magic, Magic, sizeof(char [4]));
        header.entry_count = entry_count;
        header.max_entry_count = MAX_ENTRY_COUNT;
        header.index_entry_size = sizeof(KVEntry);
        const char* kv_data_file_path = "kv_data.bin";
        header.kv_data_filepath_len = strlen(kv_data_file_path);
        fwrite(&header, sizeof(header), 1, fpidx);
        fwrite(kv_data_file_path, 1, header.kv_data_filepath_len + 1, fpidx);
    }
    fwrite(data_entry_table, MAX_ENTRY_COUNT * sizeof(KVEntry), 1, fpidx);
    free(data_entry_table);
    fclose(fpidx);

    unsigned long long hash_table[HASH_TABLE_SIZE] = {0};
    for (size_t i = 0; i < entry_count; i++) {
        hash_table[i] = hash(key[i].data, key[i].size);
        printf("hash_idx[%d]=%llu\n", i, hash_table[i] % HASH_TABLE_SIZE);
    }
    putchar('\n');
    search("User.Ethnicity", key);

    fwrite(hash_table, sizeof(unsigned long long), HASH_TABLE_SIZE, fphash);
    fclose(fphash);

    return 0;
}