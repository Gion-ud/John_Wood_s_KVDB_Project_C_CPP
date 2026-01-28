#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <stdint.h>

#define MAX_ENTRY_COUNT 16
#define HASH_TABLE_SIZE MAX_ENTRY_COUNT
#define KV_ENTRY_SIZE sizeof(KVEntry)
//#define KV_DATA_ENTRY_TABLE_SIZE (MAX_ENTRY_COUNT * sizeof(KVEntry))

static const unsigned char Magic[4] = { 'K', 'V', '\n', '\0' };

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
unsigned long hash(const unsigned char* key, size_t len) {
    unsigned long hash_val = HASH_INIT_NUM;
    for (size_t i = 0; i < len; i++) {
        hash_val ^= key[i];
        hash_val *= HASH_MUL_NUM;
    }
    return hash_val;
}
#undef HASH_INIT_NUM
#undef HASH_MUL_NUM

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
    memcpy(mem_buf.start_ptr + mem_buf.mem_buf_cur, (unsigned char*)Magic, sizeof(Magic));
    mem_buf.mem_buf_cur += sizeof(Magic);

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

#undef MEM_BUF_CUR
#undef MEM_BUF_START
#undef mem_buf
}

void print_all_kv(MemBuffer* p_mem_buf, KVEntry* data_entry_table, unsigned int entry_count) {
#define mem_buf (*p_mem_buf)
    unsigned int i = 0;
loop_start:
    if (!(i < entry_count)) goto loop_end;
    printf(
        "%s=%s\n",
        mem_buf.start_ptr + data_entry_table[i].key_offset,
        mem_buf.start_ptr + data_entry_table[i].val_offset
    );
    i++;
loop_end:
    return;
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
        "%s=%s\n",
        mem_buf.start_ptr + data_entry_table[idx].key_offset,
        mem_buf.start_ptr + data_entry_table[idx].val_offset
    );
    #undef mem_buf
}

static inline void set_str_kv(KV* key, KV* val, const char* key_str, const char* val_str) {
    strcpy((*key).data, key_str);
    key[0].size = strlen(key[0].data) + 1;
    strcpy(val[0].data, val_str);
    val[0].size = strlen(val[0].data) + 1;
}

int main() {
    FILE* fp = fopen("kv_data.bin", "wb+");
    FILE* fpidx = fopen("index.dat", "wb+");
    if (!fp || !fpidx) {
        perror("fopen");
        exit(1);
    }
    MemBuffer mem_buf = init_mem_buf(BUFFER_SIZE);
    KVEntry* data_entry_table = (KVEntry*)init_mem_kv_store(&mem_buf, MAX_ENTRY_COUNT);
    unsigned int entry_count = 0;

    KV key[MAX_ENTRY_COUNT], val[MAX_ENTRY_COUNT];

    for (unsigned int i = 0; i < MAX_ENTRY_COUNT; i++) {
        key[i].data = alloca(32);
        val[i].data = alloca(32);
    }

    /*
    strcpy(key[0].data, "User[0].FirstName");
    key[0].size = strlen(key[0].data) + 1;
    strcpy(val[0].data, "John");
    val[0].size = strlen(val[0].data) + 1;

    strcpy(key[1].data, "User[0].LastName");
    key[1].size = strlen(key[1].data) + 1;
    strcpy(val[1].data, "Wood");
    val[1].size = strlen(val[1].data) + 1;

    strcpy(key[2].data, "User[0].DateOfBirth");
    key[2].size = strlen(key[2].data) + 1;
    strcpy(val[2].data, "04/04/2009");
    val[2].size = strlen(val[2].data) + 1;

    strcpy(key[3].data, "User[0].Ethnicity");
    key[3].size = strlen(key[3].data) + 1;
    strcpy(val[3].data, "Chinese");
    val[3].size = strlen(val[3].data) + 1;

    strcpy(key[4].data, "User[0].CountryOfOrigin");
    key[4].size = strlen(key[4].data) + 1;
    strcpy(val[4].data, "China");
    val[4].size = strlen(val[4].data) + 1;
    */

    set_str_kv(&key[0], &val[0], "User.FirstName", "John");
    set_str_kv(&key[1], &val[1], "User.LastName", "Wood");
    set_str_kv(&key[2], &val[2], "User.DateOfBirth", "04/04/2009");
    set_str_kv(&key[3], &val[3], "User.Ethnicity", "Chinese");
    set_str_kv(&key[4], &val[4], "User.CountryOfOrigin", "China");

    insert_mem_kv_entry(&mem_buf, &data_entry_table[0], key[0], val[0]);
    insert_mem_kv_entry(&mem_buf, &data_entry_table[1], key[1], val[1]);
    insert_mem_kv_entry(&mem_buf, &data_entry_table[2], key[2], val[2]);
    insert_mem_kv_entry(&mem_buf, &data_entry_table[3], key[3], val[3]);
    insert_mem_kv_entry(&mem_buf, &data_entry_table[4], key[4], val[4]);


    print_kv_pair(&mem_buf, data_entry_table, 0);
    print_kv_pair(&mem_buf, data_entry_table, 1);
    print_kv_pair(&mem_buf, data_entry_table, 2);
    print_kv_pair(&mem_buf, data_entry_table, 3);
    print_kv_pair(&mem_buf, data_entry_table, 4);



    unsigned long long hash_table[HASH_TABLE_SIZE] = {0};
    hash_table[0] = hash(key[0].data, key[0].size);
    hash_table[1] = hash(key[1].data, key[1].size);
    hash_table[2] = hash(key[2].data, key[2].size);
    hash_table[3] = hash(key[3].data, key[3].size);
    hash_table[4] = hash(key[4].data, key[4].size);
    printf("hash_idx[%d]=%llu\n", 0, hash_table[0] % HASH_TABLE_SIZE);
    printf("hash_idx[%d]=%llu\n", 1, hash_table[1] % HASH_TABLE_SIZE);
    printf("hash_idx[%d]=%llu\n", 2, hash_table[2] % HASH_TABLE_SIZE);
    printf("hash_idx[%d]=%llu\n", 3, hash_table[3] % HASH_TABLE_SIZE);
    printf("hash_idx[%d]=%llu\n", 4, hash_table[4] % HASH_TABLE_SIZE);

    //write_index_table(&mem_buf, data_entry_table, MAX_ENTRY_COUNT);
    fwrite(data_entry_table, MAX_ENTRY_COUNT * sizeof(KVEntry), 1, fpidx);
    fwrite(mem_buf.start_ptr, BUFFER_SIZE, 1, fp);

    free(data_entry_table);
    destroy_mem_buf(&mem_buf);
    fclose(fp);
    fclose(fpidx);
    return 0;
}