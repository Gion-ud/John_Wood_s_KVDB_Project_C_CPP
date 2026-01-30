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

    for (int i = 0; i < dbp->Header.EntryCount; i++) { 
        KVPair *kv = ReadDBEntry(&db, i);
        if (!kv) continue;
        PrintRecordHeader(stdout, dbp, i);
        printf("\n%d\n", *(int*)kv->key.data);
        printf("%.*s\n\n", kv->val.size - 15 * 8, (char*)kv->val.data + 15 * 8);
        DestroyKVPair(kv);
    }


    CloseDB(&db);

#undef db
    return 0;
}