#include "global.h"
#include "kvdb.h"

#define prog_name argv[0]
#define input_key argv[1]
#define input_val argv[2]
#define import_filepath argv[3]
#define export_filepath argv[4]

int main(int argc, char *argv[]) {
    if (argc < 4 || argc > 5) {
        printerrf(
            "Error: Invalid argc (argc=%d)\n"
            "Usage: %s <key> <val> <import filepath> (<export filepath>)\n",
            argc, prog_name
        );
        return -1;
    }

    FILE *of_fp = NULL;
    if (argc == 4) {
        of_fp = stdout; // default
    } else if (argc == 5) {
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
        .len = strlen(input_key),
        .data = input_key
    };
    Val val = {
        .type = TYPE_TEXT,
        .len = strlen(input_val),
        .data = input_val
    };
    int ret = KVDB_DBObject_put(&db, &key, &val);
    if (ret < 0) {
        printerrf("KVDB_DBObject_put failed: ret=%d\n", ret);
        goto cleanup;
    }
    fprintf(of_fp, "db.record.key=%.*s\n", (int)key.len,(char*)key.data);
    fprintf(of_fp, "db.record.val=%.*s\n", (int)val.len,(char*)val.data);
cleanup:
    KVDB_DBObject_close(&db);
    if (of_fp && of_fp != stdout) fclose(of_fp);
#undef db
    return 0;
}