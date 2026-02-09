#include "key_table.h"

KeyTableObject *KEY_TABLE_KeyTableObject_create(size_t table_size) {
    KeyTableObject *kt_obj = (KeyTableObject*)calloc(1, sizeof(KeyTableObject));
    if (!kt_obj) {
        print_err_msg("(KeyTableObject*)calloc(1, sizeof(KeyTableObject)) failed\n");
        return NULL;
    }

    kt_obj->cap = table_size;
    kt_obj->size = 0;
    kt_obj->key_arr = (TLVDataObject*)calloc(table_size, sizeof(TLVDataObject));
    if (!kt_obj->key_arr) {
        print_err_msg("TLVDataObject*)calloc(table_size, sizeof(TLVDataObject)) failed\n");
        free(kt_obj);
        return NULL;
    }

    return kt_obj;
}
void KEY_TABLE_KeyTableObject_destroy(KeyTableObject *kt_obj) {
    if (!kt_obj) return;
    if (!kt_obj->key_arr) { free(kt_obj); }
    for (size_t i = 0; i < kt_obj->cap; ++i) {
        if (kt_obj->key_arr[i].data) free(kt_obj->key_arr[i].data);
    }
    free(kt_obj->key_arr);
    free(kt_obj);
}


int KEY_TABLE_KeyTableObject_insert(KeyTableObject *k_tbl_obj, HTObject *ht_obj, const TLVDataObject *key) {
    hash_t key_hash = hash(key->data, key->len);
    int ret = HASH_INDEX_LIB_HTObject_insert(ht_obj, key_hash, k_tbl_obj->size);
    if (ret < 0) return -1;

    k_tbl_obj->key_arr[k_tbl_obj->size].len = key->len;
    k_tbl_obj->key_arr[k_tbl_obj->size].type = key->type;
    k_tbl_obj->key_arr[k_tbl_obj->size].data = (ubyte_t*)malloc(k_tbl_obj->key_arr[k_tbl_obj->size].len);
    if (!k_tbl_obj->key_arr[k_tbl_obj->size].data) {
        print_err_msg("(ubyte_t*)malloc(k_tbl_obj->key_arr[k_tbl_obj->size].len) failed\n");
        return -1;
    }
    memcpy(k_tbl_obj->key_arr[k_tbl_obj->size].data, key->data, key->len);

    return k_tbl_obj->size++;
}

int KEY_TABLE_KeyTableObject_get_entry_id(KeyTableObject *k_tbl_obj, HTObject *ht_obj, const TLVDataObject *key) {
    if (!k_tbl_obj || !k_tbl_obj->key_arr) return -1;
    hash_t key_hash = hash(key->data, key->len);
    int hidx = key_hash % ht_obj->ht_cap;
    for (int i = 0; i < ht_obj->ht[hidx].bucket_size; ++i) {
#define key_idx ht_obj->ht[hidx].bucket[i].entry_id
        if (
            k_tbl_obj->key_arr[key_idx].len == key->len &&
            k_tbl_obj->key_arr[key_idx].type == key->type &&
            memcmp(
                k_tbl_obj->key_arr[key_idx].data,
                key->data,
                k_tbl_obj->key_arr[key_idx].len
            ) == 0
        ) {
            return key_idx;
        }
#undef key_idx
    }

    return -1;
};

int KEY_TABLE_KeyTableObject_delete(KeyTableObject *k_tbl_obj, HTObject *ht_obj, const TLVDataObject *key) {

};