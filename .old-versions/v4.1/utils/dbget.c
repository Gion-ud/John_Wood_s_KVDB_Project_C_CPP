#include "global.h"
#include "kvdb.h"

#define prog_name argv[0]
#define search_key argv[1]
#define import_filepath argv[2]
#define export_filepath argv[3]

int main(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        printerrf(
            "Error: Invalid argc (argc=%d)\n"
            "Usage: %s <key> <import filepath> (<export filepath>)\n",
            argc, prog_name
        );
        return -1;
    }

    FILE *of_fp = NULL;
    if (argc == 3) {
        of_fp = stdout; // default
    } else if (argc == 4) {
        of_fp = fopen(export_filepath, "w");
        if (!of_fp) {
            printerrf("fopen failed: %s\n", strerror(errno));
            printerrf("Using stdout instead\n");
            of_fp = stdout;
        }
    }
    DBObject* dbp = (DBObject*)KVDB_DBObject_open(import_filepath);
    if (!dbp) return 1;
#define db (*dbp)
    Key key = {
        .type = TYPE_TEXT,
        .len = strlen(search_key),
        .data = search_key
    };
    Val *val = KVDB_DBObject_get_by_key(&db, &key);
    if (!val) {
        printerrf("KVDB_DBObject_get_by_key failed\n");
        goto cleanup;
    }
    fprintf(of_fp, "db.record.val=%.*s\n", (int)val->len,(char*)val->data);
    KVDB_TLVDataObject_destroy(val);
cleanup:
    KVDB_DBObject_close(&db);
    if (of_fp && of_fp != stdout) fclose(of_fp);
#undef db
    return 0;
}