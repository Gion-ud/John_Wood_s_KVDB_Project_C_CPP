#include "kvdb_lib.h"

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 3) {
        printerrf(
            "Error: Invalid argc (argc=%d)\n"
            "Usage: %s <kvdb filepath>\n",
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
            printerrf("fopen(\"%s\", \"w\") failed: %s\n", argv[2], strerror(errno));
            printerrf("Using stdout instead\n");
            of_fp = stdout;
        }
    }
    DBObject* dbp = (DBObject*)DBOpen(argv[1]);
    if (!dbp) return 1;
#define db (*dbp)
    PrintDBFileHeader(of_fp, &db);
    for (int i = 0; i < dbp->Header.EntryCount; i++) {
        KVPair *kv = ReadDBEntry(&db, i);
        if (!kv) continue;
        PrintIndexEntry(of_fp, dbp, i);
        DestroyKVPair(kv);
    }

    uint_t val_len = 0;
    uint_t col_len = 0;
    ubyte_t *val_data_p = NULL;
    for (int i = 0; i < dbp->Header.EntryCount; i++) { 
        KVPair *kv = ReadDBEntry(&db, i);
        if (!kv) continue;
        PrintRecordHeader(of_fp, dbp, i);
        fprintf(of_fp, "\nkey=%d\n", *(int*)kv->key.data); // keys are literal integers
        val_len = kv->val.size;
        val_data_p = (ubyte_t*)kv->val.data;

        fprintf(of_fp, "val row%.4d:\n\tlen  data\n", i);
        while (val_len > 0) {
            col_len = *(ulong_t*)val_data_p;
            val_data_p += sizeof(ulong_t);
            val_len -= sizeof(ulong_t);
            fprintf(of_fp, "\t%.4u '%.*s'\n", col_len, col_len, (char*)val_data_p);
            val_data_p += col_len;
            val_len -= col_len;
        }
        fputc('\n', of_fp);
        DestroyKVPair(kv);
    }
    CloseDB(&db);
    if (argc == 3) fclose(of_fp);
#undef db
    return 0;
}