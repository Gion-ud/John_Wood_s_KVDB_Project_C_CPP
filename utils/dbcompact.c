#include "global.h"
#include "kvdb.h"
#include "kvdb_defs.h"

#define prog_name argv[0]
#define db_filepath argv[1]
#define output_db_filepath argv[2]

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printerrf(
            "Error: Invalid argc (argc=%d)\n"
            "Usage: %s <db filepath> <output db filepath>\n",
            argc, prog_name
        );
        return -1;
    }

    print_dbg_msg("KVDB_DBObject_open\n");
    DBObject *dbp = KVDB_DBObject_open(db_filepath);
    if (!dbp) {
        printerrf("KVDB_DBObject_open failed\n");
        return -1;
    }

    ulong_t db_cap = dbp->Header.EntryCapacity;
    print_dbg_msg("KVDB_DBObject_create\n");
    DBObject *dbp_new = KVDB_DBObject_create(output_db_filepath, db_cap);
    if (!dbp) {
        printerrf("KVDB_DBObject_create failed\n");
        KVDB_DBObject_close(dbp);
        return -1;
    }

    TLVDataObject *key = NULL, *val = NULL;
    int ret = 0;
    print_dbg_msg("for loop\n");
    for (ulong_t i = 0; i < dbp->Header.EntryCount; ++i) {
        print_dbg_msg("Entry%.4u\n", i);
        if (dbp->IndexTable[i].Flags & FLAG_DELETED) {
            print_dbg_msg("Entry%.4u was deleted\n", i);
            continue;
        }
        key = KVDB_DBObject_get_key(dbp, i);
        if (!key) {
            printerrf("KVDB_DBObject_get_key failed\n");
            continue;
        }
        val = KVDB_DBObject_get(dbp, i);
        if (!val) {
            printerrf("KVDB_DBObject_get failed\n");
            KVDB_TLVDataObject_destroy(val);
            continue;
        }
        ret = KVDB_DBObject_put(dbp_new, key, val);
        if (ret < 0) printerrf("KVDB_DBObject_put failed\n");
        KVDB_TLVDataObject_destroy(key);
        KVDB_TLVDataObject_destroy(val);
    }
    print_dbg_msg("endfor\n");

    KVDB_DBObject_close(dbp);
    KVDB_DBObject_close(dbp_new);

    return 0;
}