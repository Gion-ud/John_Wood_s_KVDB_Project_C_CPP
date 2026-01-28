#include "kv_lib.h"

int Init_KVArrObject(KVArrObject *kvObj, ulong_t cnt) {
    if (!kvObj) return -1;
    kvObj->cnt = 0;
    kvObj->max_cnt = cnt;
    kvObj->kv_arr = (KVEntry*)calloc(kvObj->max_cnt, sizeof(KVEntry));
    if (!kvObj->kv_arr) { perror("calloc failed"); return -1; }
    return kvObj->max_cnt;
}

int put_kv(KVArrObject *kvObj, byte_t *key, byte_t *val, ulong_t key_len, ulong_t val_len) {
    if (kvObj->cnt >= kvObj->max_cnt) {
        fputs("KV arr full\n", stderr);
        return -1;
    };
    kvObj->kv_arr[kvObj->cnt].key_data = (byte_t*)malloc(key_len);
    kvObj->kv_arr[kvObj->cnt].val_data = (byte_t*)malloc(val_len);
    if (!kvObj->kv_arr[kvObj->cnt].key_data || !kvObj->kv_arr[kvObj->cnt].val_data) {
        perror("malloc failed");
        return -1;
    }
    memcpy(kvObj->kv_arr[kvObj->cnt].key_data, key, key_len);
    memcpy(kvObj->kv_arr[kvObj->cnt].val_data, val, val_len);
    kvObj->kv_arr[kvObj->cnt].key_len = key_len;
    kvObj->kv_arr[kvObj->cnt].val_len = val_len;

    return kvObj->cnt++;
}

KVEntry *get_kv(KVArrObject *kvObj, ulong_t idx) {
    if (idx >= kvObj->max_cnt) {
        fputs("Invalid idx\n", stderr);
        return NULL;
    };
    KVEntry *kv = (KVEntry*)malloc(sizeof(KVEntry));
    if (!kv) { perror("malloc failed"); return NULL; }
    kv->key_len = kvObj->kv_arr[idx].key_len;
    kv->val_len = kvObj->kv_arr[idx].val_len;
    kv->key_data = (byte_t*)malloc(kv->key_len);
    kv->val_data = (byte_t*)malloc(kv->val_len);
    if (!kv->key_data || !kv->val_data) {
        perror("malloc failed");
        return NULL;
    }
    memcpy(kv->key_data, kvObj->kv_arr[idx].key_data, kv->key_len);
    memcpy(kv->val_data, kvObj->kv_arr[idx].val_data, kv->val_len);
    return kv;
}

void print_kv(const KVEntry *kv) {
    if (!kv) return;
    kv->key_len;
    kv->val_len;
    kv->key_data;
    kv->val_data;
    printf(
        "key_len=%u\n"
        "key_data=\"%*s\"\n"
        "val_len=%u\n"
        "val_data=\"%*s\"\n"
        "\n",
        kv->key_len,
        kv->key_len - 1, kv->key_data,
        kv->val_len,
        kv->val_len - 1, kv->val_data
    );
}

inline ulong_t get_kv_entry_size(const KVEntry *kv) {
    return 2 * sizeof(ulong_t) + kv->key_len + kv->val_len;
}

void Destroy_KVArrObject(KVArrObject *kvObj) {
    if (!kvObj) return;
    kvObj->cnt = 0;
    kvObj->max_cnt = 0;
    if (kvObj->kv_arr) free(kvObj->kv_arr);
    kvObj->kv_arr = NULL;
}

