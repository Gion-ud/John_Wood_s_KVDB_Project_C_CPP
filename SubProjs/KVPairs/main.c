#include "kv_lib.h"
const byte_t Magic[MAGIC_LEN] = { 0xD4, 'K', 'V', ' ', 'D', 'A', 'T', '\0' };

// 0xD4 = 256 - 'D' = (unsigned)-'D'
int main() {
    int ret;
    KVArrObject kvObj = {0};
    //const byte_t *key_data, *val_data;
    //ulong_t key_len, val_len;

    ret = Init_KVArrObject(&kvObj, MAX_ENTRY_COUNT);
    if (ret < 0) return 1;

    //key_data = "UserName";
    //val_data = "John_Wood";
    //key_len = strlen(key_data) + 1;
    //val_len = strlen(val_data) + 1;
    insert_kv_str(&kvObj, "UserName", "John_Wood");
    insert_kv_str(&kvObj, "FirstName", "John");
    insert_kv_str(&kvObj, "LastName", "Wood");
    insert_kv_str(&kvObj, "Email", "chiunyuwu@gmail.com");
    insert_kv_str(&kvObj, "Country", "New Zealand");
    insert_kv_str(&kvObj, "City", "Auckland");
    insert_kv_str(&kvObj, "Suburb", "Howick");
    insert_kv_str(&kvObj, "CountryOfOrigin", "China");
    insert_kv_str(&kvObj, "PhoneNumber", "0277686808");
    insert_kv_str(&kvObj, "Ethnicity", "Asian/Chinese");
    insert_kv_str(&kvObj, "Gender", "Male");
    insert_kv_str(&kvObj, "DateOfBirth", "04Apr2009");


    FILE *fpdat = fopen("kv.dat", "wb+");
    //FILE *fpidx = fopen("kv.idx", "wb+");
    if (!fpdat) { perror("fopen failed"); Destroy_KVArrObject(&kvObj); return 1; }
    //ulong_t Offptr = 0;
    fwrite(Magic, 1, MAGIC_LEN, fpdat);
    fwrite(&kvObj.cnt, sizeof(ulong_t), 1, fpdat);
    fseek(fpdat, DATA_OFF, SEEK_SET);
    for (ulong_t i = 0; i < kvObj.cnt; i++) {
        KVEntry *kv = get_kv(&kvObj, i);
        // ulong_t size = get_kv_entry_size(&kvObj);
        fwrite(&kv->key_len, sizeof(ulong_t), 1, fpdat);
        fwrite(&kv->val_len, sizeof(ulong_t), 1, fpdat);
        fwrite(kv->key_data, 1, kv->key_len, fpdat);
        fwrite(kv->val_data, 1, kv->val_len, fpdat);

        //fwrite(&Offptr, sizeof(ulong_t), 1, fpidx);
        //Offptr += get_kv_entry_size(kv);
        print_kv(kv);
        free(kv->key_data);
        free(kv->val_data);
        free(kv);
    }
    rewind(fpdat);
    fclose(fpdat);
    //fclose(fpidx);
    Destroy_KVArrObject(&kvObj);
    return 0;
}