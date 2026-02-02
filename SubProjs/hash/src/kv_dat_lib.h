#ifndef KV_DAT_LIB_H
#define KV_DAT_LIB_H
//#pragma once

#include "global_utils.h"

#define MAGIC_LEN 8

typedef struct KVPair {
    ulong_t key_len;
    ubyte_t *key_data;
    ulong_t val_len;
    ubyte_t *val_data;
} KVPair;

typedef struct KVObject {
    struct KVPair *kv_arr;
    ulong_t        kv_cnt;
    ulong_t        max_kv_cnt;
} KVObject;

#pragma pack(push, 1)
typedef struct KV_DAT_FILE_HEADER {
    ubyte_t  magic[MAGIC_LEN];   // 8 bytes
    ulong_t entry_cnt;          // 4 bytes
    ulong_t data_off;           // 4 bytes
} KV_DAT_FILE_HEADER;
#pragma pack(pop)

KVObject *InitKVPairObj(ulong_t max_kv_cnt);
int KVObject_put_kv_pair(KVObject *pObj, const ubyte_t *key_data, const ubyte_t *val_data, ulong_t key_len, ulong_t val_len);
KVPair KVObject_get_kv(KVObject *pObj, ulong_t idx);
void DestroyKVPairObj(KVObject* pObj);
int KVObject_save_kv_dat(const char* filepath, KVObject *kvObj);
KVObject *KVObject_load_kv_dat(const char* filepath);


#endif