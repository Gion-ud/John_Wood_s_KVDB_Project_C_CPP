#include "../core/src/kvdb_lib.h"

int main() {
    DBObject* dbp = (DBObject*)DBOpen("database/table0002.db");
    if (!dbp) return 1;
#define db (*dbp)
    PrintDBFileHeader(stdout, &db);
    for (int i = 0; i < dbp->Header.EntryCount; i++) {
        KVPair *kv = ReadDBEntry(&db, i);
        if (!kv) continue;
        PrintIndexEntry(stdout, dbp, i);
        DestroyKVPair(kv);
    }

    uint_t val_len = 0;
    uint_t col_len = 0;
    ubyte_t *val_data_p = NULL;
    for (int i = 0; i < dbp->Header.EntryCount; i++) { 
        KVPair *kv = ReadDBEntry(&db, i);
        if (!kv) continue;
        PrintRecordHeader(stdout, dbp, i);
        printf("\nkey=%d\n", *(int*)kv->key.data); // keys are literal integers
        val_len = kv->val.size;
        val_data_p = (ubyte_t*)kv->val.data;

        printf("val row%.4d:\n\tlen  data\n", i);
        while (val_len > 0) {
            col_len = *(ulong_t*)val_data_p;
            val_data_p += sizeof(ulong_t);
            val_len -= sizeof(ulong_t);
            printf("\t%.4u '%.*s'\n", col_len, col_len, (char*)val_data_p);
            val_data_p += col_len;
            val_len -= col_len;
        }
        putchar('\n');
        DestroyKVPair(kv);
    }
    CloseDB(&db);

#undef db
    return 0;
}