#include "kv_lib.h"

int main() {
    KVArrObject kvObj = {0};
    FILE *fpdat = fopen("kv.dat", "rb");
    if (!fpdat) { perror("fopen failed"); return 1; }

    char magic_bytes[MAGIC_LEN] = {0};
    fread(magic_bytes, 1, MAGIC_LEN, fpdat);
    printf("Magic: %s\n", magic_bytes);

    ulong_t cnt = 0;
    fread(&cnt, sizeof(ulong_t), 1, fpdat);
    printf("EntryCount: %u\n\n", cnt);

    Init_KVArrObject(&kvObj, (cnt + 0b00001111) & ~0b00001111);

    fseek(fpdat, DATA_OFF, SEEK_SET);
    for (ulong_t i = 0; i < cnt; i++) {
        KVEntry kv = {0};
        fread(&kv.key_len, sizeof(ulong_t), 1, fpdat);
        fread(&kv.val_len, sizeof(ulong_t), 1, fpdat);
        kv.key_data = (byte_t*)malloc(kv.key_len);
        kv.val_data = (byte_t*)malloc(kv.val_len);
        if (!kv.key_data || !kv.val_data) {
            if (kv.key_data) free(kv.key_data);
            if (kv.val_data) free(kv.val_data);
            continue;
        }
        fread(kv.key_data, kv.key_len, 1, fpdat);
        fread(kv.val_data, kv.val_len, 1, fpdat);
        put_kv(&kvObj, kv.key_data, kv.val_data, kv.key_len, kv.val_len);
        free(kv.key_data);
        free(kv.val_data);
    }
    fclose(fpdat);

    for (ulong_t i = 0; i < kvObj.cnt; i++) {
        KVEntry *kv = get_kv(&kvObj, i);
        print_kv(kv);
        free(kv->key_data);
        free(kv->val_data);
        free(kv);
    }

    Destroy_KVArrObject(&kvObj);
    return 0;
}