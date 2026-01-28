#include "../core/src/kvdb_lib.h"

int main() {
    DBObject* dbp = (DBObject*)DBOpen("database/table0002.db");
    if (!dbp) return 1;
#define db (*dbp)

    for (int i = 0; i < dbp->Header.EntryCount; i++) { 
        KVPair *kv = ReadDBEntry(&db, i);
        PrintIndexEntry(stdout, dbp, i);
        PrintRecordHeader(stdout, dbp, i);
        if (!kv) continue;

        printf("\n%d\n", *(int*)kv->key.data);
        printf("%.*s\n", kv->val.size - 15 * 8, (char*)kv->val.data + 15 * 8);
        PrintKvPair(kv);
        DestroyKVPair(kv);
    }



    CloseDB(&db);

#undef db
    return 0;
}