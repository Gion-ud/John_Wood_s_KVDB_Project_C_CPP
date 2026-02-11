#include "global.h"
#include "kvdb_lib.h"

int main(int argc, char *argv[]) {
    if (argc == 1) {
        printf(
            "Usage: %s <kvdb filepath> (<db dump filepath>)\n",
            argv[0]
        );
        return 0;
    }
    if (argc < 2 || argc > 3) {
        printerrf(
            "Error: Invalid argc (argc=%d)\n"
            "Usage: %s <kvdb filepath> (<db dump filepath>)\n",
            argc, argv[0]
        );
        return -1;
    }
    FILE *of_fp = NULL;
    if (argc == 2) {
        of_fp = stdout; // default
    } else if (argc == 3) {
        of_fp = fopen(argv[2], "w");
        if (!of_fp) {
            printerrf("fopen failed: %s\n", strerror(errno));
            printerrf("Using stdout instead\n");
            of_fp = stdout;
        }
    }
    DBObject* dbp = (DBObject*)KVDB_DBObject_open(argv[1]);
    if (!dbp) return 1;
#define db (*dbp)
    PrintDBFileHeader(of_fp, &db);
    for (ulong_t i = 0; i < dbp->Header.EntryCount; i++) {
        KVPair *kv = KVDB_DBObject_get(&db, i);
        if (!kv) continue;
        PrintIndexEntry(of_fp, dbp, i);
        KVDB_DestroyKVPair(kv);
    }

    uint_t val_len = 0;
    uint_t col_len = 0;
    ubyte_t *val_data_p = NULL;
    for (ulong_t i = 0; i < dbp->Header.EntryCount; i++) { 
        KVPair *kv = KVDB_DBObject_get(&db, i);
        if (!kv) continue;
        PrintRecordHeader(of_fp, dbp, i);
        fprintf(of_fp, "\ndb.record%.4u.key=\'%.*s\'\n", i, (int)kv->key.len,(char*)kv->key.data); // keys are literal integers
        val_len = kv->val.len;
        val_data_p = (ubyte_t*)kv->val.data;

        fprintf(of_fp, "db.record%.4u.row:\n%8s %4s %4s\n", i, "col_id", "len", "data");
        int j = 0;
        while (val_len > 0) {
            col_len = *(ulong_t*)val_data_p;
            val_data_p += sizeof(ulong_t);
            val_len -= sizeof(ulong_t);
            fprintf(of_fp, "%8u %4u '%.*s'\n", j++, col_len, col_len, (char*)val_data_p);
            val_data_p += col_len;
            val_len -= col_len;
        }
        fputc('\n', of_fp);
        KVDB_DestroyKVPair(kv);
    }
    KVDB_DBObject_close(&db);
    if (argc == 3) fclose(of_fp);
#undef db
    return 0;
}