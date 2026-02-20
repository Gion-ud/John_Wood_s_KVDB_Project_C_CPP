#include "global.h"
#include "kvdb.h"

#define prog_name argv[0]
#define entry_id_str argv[1]
#define import_filepath argv[2]

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printerrf(
            "Error: Invalid argc (argc=%d)\n"
            "Usage: %s <entry id> <import filepath>\n",
            argc, prog_name
        );
        return -1;
    }

    FILE *of_fp = of_fp = stdout;

    DBObject* dbp = (DBObject*)KVDB_DBObject_open(import_filepath);
    if (!dbp) return 1;
#define db (*dbp)
    int ret = KVDB_DBObject_delete(&db, atoi(argv[1]));
    if (ret < 0) {
        printerrf("KVDB_DBObject_del_by_id failed\n");
        goto cleanup;
    }
cleanup:
    KVDB_DBObject_close(&db);
#undef db
    return 0;
}