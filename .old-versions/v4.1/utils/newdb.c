#include "kvdb.h"
#include "txt_tok_lib.h"

#define DB_ENTRY_CAP 4096

#define prog_name argv[0]
#define db_filepath argv[1]
#define db_cap_str argv[2]

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printerrf(
            "Error: Invalid argc (argc=%d)\n"
            "Usage: %s <db path> <db cap>\n",
            argc, prog_name
        );
        return -1;
    }

    int db_cap = atoi(db_cap_str);
    DBObject* dbp = (DBObject*)KVDB_DBObject_create(db_filepath, db_cap);
    if (!dbp) return -1;

    //PrintIndexTable(stdout, dbp);
    /*
    KVDB_DBObject_delete(&db, 1);
    KVDB_DBObject_delete(&db, 10);
    KVDB_DBObject_delete(&db, 11);
    KVDB_DBObject_delete(&db, 63);
    KVDB_DBObject_delete(&db, 14);
    KVDB_DBObject_delete(&db, 7);
    KVDB_DBObject_delete(&db, 12000);
    */

    //close_file_hash_table(&db);
    KVDB_DBObject_close(dbp);
    return 0;
}