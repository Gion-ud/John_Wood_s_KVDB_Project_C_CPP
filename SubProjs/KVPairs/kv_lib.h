#ifndef KV_LIB_H
#define KV_LIB_H

#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


typedef unsigned int  ulong_t;
typedef unsigned char byte_t;

typedef struct KVEntry {
    byte_t *key_data;
    byte_t *val_data;
    ulong_t key_len;
    ulong_t val_len;
} KVEntry;

typedef struct {
    KVEntry *kv_arr;
    ulong_t  cnt;
    ulong_t  max_cnt;
} KVArrObject;

#define MAX_ENTRY_COUNT 16

int Init_KVArrObject(KVArrObject *kvObj, ulong_t cnt);
int put_kv(KVArrObject *kvObj, byte_t *key, byte_t *val, ulong_t key_len, ulong_t val_len);
KVEntry *get_kv(KVArrObject *kvObj, ulong_t idx);
void print_kv(const KVEntry *kv);
ulong_t get_kv_entry_size(const KVEntry *kv);
void Destroy_KVArrObject(KVArrObject *kvObj);

#define insert_kv_str(pKvObj, key, val) put_kv(pKvObj, key, val, strlen(key) + 1, strlen(val) + 1);

#define MAGIC_LEN 8
#define DATA_OFF  MAGIC_LEN + sizeof(ulong_t)

#endif