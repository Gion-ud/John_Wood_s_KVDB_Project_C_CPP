#ifndef KEY_TABLE_H
#define KEY_TABLE_H

#include "global.h"
#include "hash_index_lib.h"

typedef struct _tlv_obj     TLVDataObject;
typedef struct _key_tbl_obj KeyTableObject;

struct _tlv_obj {
    size32_t    len;
    ulong_t     type;
    void       *data;
};

struct _key_tbl_obj {
    TLVDataObject *key_arr;
    size_t cap;
    size_t size;
};

#define hash(key, key_len) fnv_1a_hash(key, key_len)


KeyTableObject *KEY_TABLE_KeyTableObject_create(size_t table_size);
void KEY_TABLE_KeyTableObject_destroy(KeyTableObject *kt_obj);
int KEY_TABLE_KeyTableObject_insert(KeyTableObject *k_tbl_obj, HTObject *ht_obj, const TLVDataObject *key);
int KEY_TABLE_KeyTableObject_delete(KeyTableObject *k_tbl_obj, HTObject *ht_obj, const TLVDataObject *key);
int KEY_TABLE_KeyTableObject_get_entry_id(KeyTableObject *k_tbl_obj, HTObject *ht_obj, const TLVDataObject *key);


#endif