#include "global.h"
#include "kvdb.h"

#define prog_name argv[0]
#define import_filepath argv[1]
#define export_filepath argv[2]

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        printerrf(
            "Error: Invalid argc (argc=%d)\n"
            "Usage: %s <import filepath> (<export filepath>)\n",
            argc, prog_name
        );
        return -1;
    }

    FILE *of_fp = NULL;
    if (argc == 2) {
        of_fp = stdout; // default
    } else if (argc == 3) {
        of_fp = fopen(export_filepath, "w");
        if (!of_fp) {
            printerrf("fopen failed: %s\n", strerror(errno));
            printerrf("Using stdout instead\n");
            of_fp = stdout;
        }
    }
    DBObject* dbp = (DBObject*)KVDB_DBObject_open(import_filepath);
    if (!dbp) {
        print_err_msg("KVDB_DBObject_open failed\n");
        return -1;
    }
#define db (*dbp)
    for (ulong_t i = 0; i < KVDB_DBObject_EntryCount(&db); i++) {
        Key *key = KVDB_DBObject_get_key(&db, i);
        if (!key) continue;
        Val *val = KVDB_DBObject_get(&db, i);
        if (!val) { KVDB_TLVDataObject_destroy(key); continue; }

        printf("Entry%.4u ", i);
        fprintf(of_fp, "%.*s=%.*s\n",
            (int)key->len,(char*)key->data,
            (int)val->len,(char*)val->data
        );
        if (of_fp != stdout) putchar('\n');

        KVDB_TLVDataObject_destroy(key);
        KVDB_TLVDataObject_destroy(val);
    }
    KVDB_DBObject_close(&db);
    if (of_fp && of_fp != stdout) fclose(of_fp);
#undef db
    return 0;
}