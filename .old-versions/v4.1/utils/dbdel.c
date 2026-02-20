#include "global.h"
#include "kvdb.h"

#define prog_name argv[0]
#define lookup_key argv[1]
#define import_filepath argv[2]

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printerrf(
            "Error: Invalid argc (argc=%d)\n"
            "Usage: %s <key> <import filepath>\n",
            argc, prog_name
        );
        return -1;
    }

    FILE *of_fp = of_fp = stdout;

    DBObject* dbp = (DBObject*)KVDB_DBObject_open(import_filepath);
    if (!dbp) return 1;
#define db (*dbp)
    Key key = {
        .type = TYPE_TEXT,
        .len = strlen(lookup_key),
        .data = lookup_key
    };
    int ret = KVDB_DBObject_delete_by_key(&db, &key);
    if (ret < 0) {
        printerrf("KVDB_DBObject_del_by_key failed\n");
        goto cleanup;
    }
cleanup:
    KVDB_DBObject_close(&db);
#undef db
    return 0;
}