#include "kv_dat_lib.h"

const byte_t Magic[MAGIC_LEN] = { 'K', 'V', ' ', 'D', 'A', 'T', '\r', '\n' };

KVObject *InitKVPairObj(ulong_t max_kv_cnt) {
    KVObject *pObj = (KVObject*)malloc(sizeof(KVObject));
    if (!pObj) { print_err_msg("(KVObject*)malloc(sizeof(KVObject)) failed\n"); return NULL; }
    pObj->kv_arr = (KVPair*)calloc(max_kv_cnt, sizeof(KVPair));
    if (!pObj->kv_arr) {
        print_err_msg("(KVPair*)calloc(MAX_ENTRY_COUNT, sizeof(KVPair)) failed\n");
        return NULL;
    }
    pObj->kv_cnt = 0;
    pObj->max_kv_cnt = max_kv_cnt;
    return pObj;
}

int KVObject_put_kv_pair(KVObject *pObj, const byte_t *key_data, const byte_t *val_data, ulong_t key_len, ulong_t val_len) {
#define idx pObj->kv_cnt
    if (!pObj || !pObj->kv_arr) { print_err_msg("Error: (KVObject*) pObj uninitialised\n"); return -1; }
    if (idx >= pObj->max_kv_cnt) { print_err_msg("Error: kv arr full\n"); return -2; };
    pObj->kv_arr[idx].key_len = key_len;
    pObj->kv_arr[idx].val_len = val_len;

    pObj->kv_arr[idx].key_data = (byte_t*)malloc(key_len);
    if (!pObj->kv_arr[idx].key_data) { print_err_msg("(byte_t*)malloc(key_len) failed\n"); return -3; }

    pObj->kv_arr[idx].val_data = (byte_t*)malloc(val_len);
    if (!pObj->kv_arr[idx].val_data) { print_err_msg("(byte_t*)malloc(val_len) failed\n"); free(pObj->kv_arr[idx].key_data); return -3; }
    memcpy(pObj->kv_arr[idx].key_data, key_data, key_len);
    memcpy(pObj->kv_arr[idx].val_data, val_data, val_len);
    return idx++;
#undef idx
}

KVPair KVObject_get_kv(KVObject *pObj, ulong_t idx) {
    KVPair kv = {0};
    memcpy(&kv, &pObj->kv_arr[idx], sizeof(KVPair));
    return kv;
}

void DestroyKVPairObj(KVObject* pObj) {
    if (!pObj) return;
    if (!pObj->kv_arr) { free(pObj); return; }
    for (int i = 0; i < pObj->max_kv_cnt; i++) {
        if (pObj->kv_arr[i].key_data) free(pObj->kv_arr[i].key_data);
        if (pObj->kv_arr[i].val_data) free(pObj->kv_arr[i].val_data);
    }
    free(pObj->kv_arr);
    free(pObj);
}


int KVObject_save_kv_dat(const char* filepath, KVObject *kvObj) {
    FILE *fp = fopen(filepath, "wb+");
    if (!fp) { perror("fopen"); return -1; }

    fwrite(Magic, sizeof(byte_t), MAGIC_LEN, fp);
    fwrite(&kvObj->kv_cnt, sizeof(kvObj->kv_cnt), 1, fp);
    ulong_t data_off = sizeof(KV_DAT_FILE_HEADER);
    fwrite(&data_off, sizeof(data_off), 1, fp);

    fseek(fp, data_off, SEEK_SET);
//\
    # binary dat file layout: [header], then ([key_hash])[key_len][key_data][val_len][val_data]
    for (ulong_t i = 0; i < kvObj->kv_cnt; i++) {
        KVPair kv = KVObject_get_kv(kvObj, i);
        //hash_t key_hash = HTLib_FNV_1a_hash(kv.key_data, kv.key_len);
        //fwrite(&key_hash, sizeof(key_hash), 1, fp);
        fwrite(&kv.key_len, sizeof(kv.key_len), 1, fp);
        fwrite(kv.key_data, sizeof(byte_t), kv.key_len, fp);
        fwrite(&kv.val_len, sizeof(kv.val_len), 1, fp);
        fwrite(kv.val_data, sizeof(byte_t), kv.val_len, fp);
    }
    fclose(fp);
    return kvObj->kv_cnt;
}

KVObject *KVObject_load_kv_dat(const char* filepath) {
    KVObject *kvObj = NULL;
    FILE *fp = fopen(filepath, "rb");
    if (!fp) { perror("fopen"); goto err_cleanup; }

    KV_DAT_FILE_HEADER kv_dat_hdr = {0};
    fread(&kv_dat_hdr, sizeof(KV_DAT_FILE_HEADER), 1, fp);
    if (memcmp(kv_dat_hdr.magic, Magic, MAGIC_LEN) != 0) {
        print_err_msg("Error: Invalid file format\n");
        goto err_cleanup;
    }

    kvObj = InitKVPairObj((kv_dat_hdr.entry_cnt + 0b00001111) & ~0b00001111);
    if (!kvObj) goto err_cleanup;

    fseek(fp, kv_dat_hdr.data_off, SEEK_SET);
    print_dbg_msg("[DEBUG] kv_dat_hdr.data_off=%u\n", kv_dat_hdr.data_off);
//\
    # binary dat file layout: [header], then [key_hash][key_len][key_data][val_len][val_data]
    for (ulong_t i = 0; i < kv_dat_hdr.entry_cnt; i++) {
        //hash_t key_hash;
        ulong_t key_len, val_len;

        //fread(&key_hash, sizeof(key_hash), 1, fp);

        fread(&key_len, sizeof(key_len), 1, fp);
        byte_t *key_data = (byte_t*)malloc(key_len);
        if (!key_data) { perror("malloc"); goto err_cleanup; }
        fread(key_data, sizeof(byte_t), key_len, fp);

        fread(&val_len, sizeof(val_len), 1, fp);
        byte_t *val_data = (byte_t*)malloc(val_len);
        if (!val_data) { perror("malloc"); free(key_data); goto err_cleanup; }
        fread(val_data, sizeof(byte_t), val_len, fp);

        int ret = KVObject_put_kv_pair(kvObj, key_data, val_data, key_len, val_len);
        free(key_data); free(val_data);
        if (ret != i) goto err_cleanup;
    }
    fclose(fp);
    return kvObj;
err_cleanup:
    if (kvObj) DestroyKVPairObj(kvObj);
    return NULL;
}