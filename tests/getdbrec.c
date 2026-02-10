#include "kvdb_lib.h"

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf(
            "Usage: %s <key> <kvdb filepath> (<output file>)\n",
            argv[0]
        );
        return 0;
    }
    if (argc < 3 || argc > 4) {
        printerrf(
            "Error: Invalid argc (argc=%d)\n"
            "Usage: %s <key> <kvdb filepath> (<output file>)\n",
            argc, argv[0]
        );
        return -1;
    }
    FILE *of_fp = NULL;
    if (argc == 3) {
        of_fp = stdout; // default
    } else if (argc == 4) {
        of_fp = fopen(argv[3], "w");
        if (!of_fp) {
            printerrf("fopen failed: %s\n", strerror(errno));
            printerrf("Using stdout instead\n");
            of_fp = stdout;
        }
    }
    DBObject* dbp = (DBObject*)KVDB_DBObject_open(argv[2]);
    if (!dbp) return 1;
#define db (*dbp)
    Key k = { strlen(argv[1]), BLOB, NULL };
    k.data = (char*)malloc(k.len);
    if (!k.data) {
        print_err_msg("malloc failed\n");
        goto cleanup;
    }
    memcpy(k.data, argv[1], k.len);


    KVPair *kv = KVDB_DBObject_get(&db, 0);
    if (!kv) goto cleanup;
    ulong_t val_len = kv->val.len;
    ulong_t col_len;
    ubyte_t *val_data_p = (ubyte_t*)kv->val.data;
    int i = 0;
    fprintf(of_fp, "db_table_header:\n%8s %4s\n", "col_id", "col_name");
    while (val_len > 0) {
        col_len = *(ulong_t*)val_data_p;
        val_data_p += sizeof(ulong_t);
        val_len -= sizeof(ulong_t);
        fprintf(of_fp, "%8u '%.*s'\n", i++, col_len, (char*)val_data_p);
        val_data_p += col_len;
        val_len -= col_len;
    }
    fputc('\n', of_fp);
    free(kv);

    kv = KVDB_DBObject_get_by_key(&db, k);
    if (!kv) goto cleanup;
    val_len = kv->val.len;
    val_data_p = (ubyte_t*)kv->val.data;

    fprintf(of_fp, "record:\n%8s %4s %4s\n", "col_id", "len", "data");
    i = 0;
    while (val_len > 0) {
        col_len = *(ulong_t*)val_data_p;
        val_data_p += sizeof(ulong_t);
        val_len -= sizeof(ulong_t);
        fprintf(of_fp, "%8u %4u '%.*s'\n", i++, col_len, col_len, (char*)val_data_p);
        val_data_p += col_len;
        val_len -= col_len;
    }
    fputc('\n', of_fp);
    //fwrite((char*)kv->val.data, kv->val.len, 1, of_fp);
    free(kv);
cleanup:
    KVDB_DBObject_close(&db);
    if (argc == 3) fclose(of_fp);
#undef db
    return 0;
}