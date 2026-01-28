#include "kvdb_lib.h"

#define DB_MAX_ENTRY_COUNT 32

int main() {
    DBObject* dbp = (DBObject*)DBInit("database/kv_store.db", DB_MAX_ENTRY_COUNT);
//#define db (*dbp)
    KV key, val;

    key.data = "username";
    key.size = strlen(key.data) + 1;
    key.type = STRING;

    val.data = "John_Wood";
    val.size = strlen(val.data) + 1;
    val.type = STRING;

    InsertEntry(dbp, key, val);

    ReadDBEntryKey(dbp, key);



    CloseDB(dbp);
//#undef db
    return 0;
}