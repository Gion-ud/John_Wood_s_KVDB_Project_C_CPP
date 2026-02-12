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
    if (!dbp) return 1;
#define db (*dbp)
    PrintDBFileHeader(of_fp, &db);
    PrintIndexTable(of_fp, &db);
    for (ulong_t i = 0; i < dbp->Header.EntryCount; i++) { 
        KVPair *kv = KVDB_DBObject_get(&db, i);
        if (!kv) continue;
        PrintRecordHeader(of_fp, dbp, i);

        fprintf(of_fp, "db.record%.4u.key=%.*s\n", i, (int)kv->key.len,(char*)kv->key.data);
        fprintf(of_fp, "db.record%.4u.val=%.*s\n\n", i, (int)kv->val.len,(char*)kv->val.data);

        KVDB_DestroyKVPair(kv);
    }
    KVDB_DBObject_close(&db);
    if (argc == 3) fclose(of_fp);
#undef db
    return 0;
}