#include "hash_table_lib.h"
#include "kv_dat_lib.h"

const char *keys[] = {
    "user.UserName",
    "user.FirstName",
    "user.LastName",
    "user.NSN",
    "user.DateOfBirth",
    "user.Nationality",
    "user.CountryOfOrigin",
    "user.Race",
    "user.CountryLivingIn",
    "user.BloodType",
    "user.School",
    "user.City",
    "user.Suburb",
    "user.Gender",
    "user.HasDriversLicence",
    "user.Hobby"
};
#define KEY_CNT sizeof(keys) / sizeof(char*)

const char *vals[] = {
    "John_Wood",
    "John",
    "Wu",
    "16904120",
    "2009-04-04",
    "CN",
    "China",
    "East Asian/Chinese",
    "New Zealand",
    "Unknown",
    "Pakuranga College",
    "Auckland",
    "Howick",
    "Male",
    "true",
    "C Programing, osu!"
};
#define VAL_CNT sizeof(vals) / sizeof(char*)

#define ENTRY_COUNT KEY_CNT

#define MAX_ENTRY_COUNT (ENTRY_COUNT + 31) & ~31

int main() {
    if (KEY_CNT != VAL_CNT) { print_err_msg("Error: key val count mismatch\n"); return 1; };

Section1:
    HashTableObject htObj = {0};
    ulong_t kv_idx_arr[MAX_ENTRY_COUNT] = {0}; // This is an arr of idx of kv arr with idx being hash table idx
    KVObject *kvObj = InitKVPairObj(MAX_ENTRY_COUNT);
    if (!kvObj) goto main_cleanup;

    int ret = InitHashTableObject(&htObj, MAX_ENTRY_COUNT);
    if (ret < 0) goto main_cleanup;
 
    for (unsigned int i = 0; i < ENTRY_COUNT; i++) {
        ret = KVObject_put_kv_pair(kvObj, keys[i], vals[i], strlen(keys[i]), strlen(vals[i]));
        if (ret < 0) { print_err_msg("put kv pair failed\n"); continue; }
        KVPair kv = KVObject_get_kv(kvObj, i);
        hidx_t idx = HashTableObject_insert_entry(&htObj, kv.key_data, kv.key_len);
        kv_idx_arr[idx] = i;
        if (idx < 0) continue;
    }
    for (unsigned int i = 0; i < ENTRY_COUNT; i++) {
        hidx_t idx = HashTableObject_search_key(&htObj, kvObj->kv_arr[i].key_data, kvObj->kv_arr[i].key_len);
        KVPair kv = KVObject_get_kv(kvObj, kv_idx_arr[idx]);
        printf("val[\"%.*s\"] = \"%.*s\"\n", kv.key_len, kv.key_data, kv.val_len, kv.val_data);
    }

    ret = KVObject_save_kv_dat("cache/kv_data_ser.dat", kvObj);
    if (ret < 0) goto main_cleanup;
    DestroyHashTableObject(&htObj);
    DestroyKVPairObj(kvObj);

Section2:
    putchar('\n');
    kvObj = KVObject_load_kv_dat("cache/kv_data_ser.dat");
    if (!kvObj) goto main_cleanup;
    ret = InitHashTableObject(&htObj, kvObj->max_kv_cnt);
    if (ret < 0) goto main_cleanup;
    memset((ulong_t*)kv_idx_arr, 0, sizeof(kv_idx_arr));

    for (ulong_t i = 0; i < kvObj->kv_cnt; i++) {
        KVPair kv = KVObject_get_kv(kvObj, i);
        printf(
            "%.04d: key=%-32.*s;val=\"%.*s\"\n",
            i,
            kv.key_len,kv.key_data, kv.val_len,kv.val_data
        );
    }

    for (ulong_t i = 0; i < kvObj->kv_cnt; i++) {
        KVPair kv = KVObject_get_kv(kvObj, i);
        hidx_t idx = HashTableObject_insert_entry(&htObj, kv.key_data, kv.key_len);
        kv_idx_arr[idx] = i;
    }
    putchar('\n');

    for (unsigned int i = 0; i < ENTRY_COUNT; i++) {
        hidx_t idx = HashTableObject_search_key(&htObj, kvObj->kv_arr[i].key_data, kvObj->kv_arr[i].key_len);
        KVPair kv = KVObject_get_kv(kvObj, kv_idx_arr[idx]);
        printf("val[\"%.*s\"] = \"%.*s\"\n", kv.key_len, kv.key_data, kv.val_len, kv.val_data);
    }


main_cleanup:
    //if (fp) fclose(fp);
    DestroyHashTableObject(&htObj);
    DestroyKVPairObj(kvObj);
    return 0;
}