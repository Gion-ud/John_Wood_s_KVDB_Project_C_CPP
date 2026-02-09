#include "typeflags.h"
#include "hash_func_module.h"
#include "hash_index_lib.h"
#include "key_table.h"

#define LINE_SIZE   64
#define KEY_LEN     36

static inline size_t strip_line_crlf(char *line) {
    size_t line_len = strlen(line);
    if (line[line_len] == '\n') line[line_len--] = '\0';
    if (line[line_len] == '\r') line[line_len--] = '\0';
    return line_len;
}

int main(int argc, char *argv[]) {
    FILE *fp = fopen("uuid.txt", "r");
    if (!fp) {
        perror("fopen failed");
        return errno;
    }
    char line[LINE_SIZE];
    size_t line_len;

    int key_cnt = 0;
    while (fgets(line, LINE_SIZE, fp)) ++key_cnt;
    rewind(fp);

    char (*key)[KEY_LEN] = (char (*)[KEY_LEN])calloc(key_cnt, sizeof(char [KEY_LEN]));

{
    int i = 0;
    while (fgets(line, LINE_SIZE, fp)) {
        size_t line_len = strip_line_crlf(line);
        memcpy((char*)key[i++], line, KEY_LEN);
    }
}
    for (int i = 0; i < key_cnt; ++i) {
        printf("key%.4d: %.*s\n", i, (int)KEY_LEN, key[i]);
    }

    HTObject *ht_obj = HASH_INDEX_LIB_HTObject_create(key_cnt);
    KeyTableObject *kt_obj = KEY_TABLE_KeyTableObject_create(key_cnt);

    if (!ht_obj) return -1;
    int ret = -1;
    int idx = -1;

    TLVDataObject tlv_key = {0};
    for (int i = 0; i < key_cnt; ++i) {
        tlv_key.len = KEY_LEN;
        tlv_key.type = TYPE_TEXT;
        tlv_key.data = (char*)key[i];


        ret = KEY_TABLE_KeyTableObject_insert(kt_obj, ht_obj, &tlv_key);
        if (ret < 0) continue;
    }
    putchar('\n');
    for (int i = 0; i < key_cnt; ++i) {
        tlv_key.len = KEY_LEN;
        tlv_key.type = TYPE_TEXT;
        tlv_key.data = (char*)key[i];
        idx = KEY_TABLE_KeyTableObject_get_entry_id(kt_obj, ht_obj, &tlv_key);
        if (idx < 0) continue;
        printf("%.*s\n", KEY_LEN, kt_obj->key_arr[idx].data);
    }


    HASH_INDEX_LIB_HTObject_destroy(ht_obj);
    KEY_TABLE_KeyTableObject_destroy(kt_obj);

    free(key);
    fclose(fp);
    return 0;
}