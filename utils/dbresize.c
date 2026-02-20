#include "global.h"
#include "kvdb.h"
#include "kvdb_defs.h"

#define prog_name argv[0]
#define db_filepath argv[1]
#define db_cap_str argv[2]

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printerrf(
            "Error: Invalid argc (argc=%d)\n"
            "Usage: %s <db filepath> <new size>\n",
            argc, prog_name
        );
        return -1;
    }

    DBObject *dbp = KVDB_DBObject_open(db_filepath);
    if (!dbp) {
        printerrf("KVDB_DBObject_open failed\n");
        return -1;
    }

    int new_cap = atoi(db_cap_str);
    if (new_cap <= (int)dbp->Header.EntryCount) {
        printerrf(
            "Error: not enough capacity\n"
            "new capacity must be at least %u\n",
            dbp->Header.EntryCount + 1
        );
        KVDB_DBObject_close(dbp);
        return -1;
    }
    printf(
        "old_cap=%d\nnew_cap=%d\n",
        (int)dbp->Header.EntryCapacity, new_cap
    );
    dbp->Header.EntryCapacity = (ulong_t)new_cap;
    //dbp->db_modified = 1;
    KVDB_DBObject_close(dbp);

    return 0;
}