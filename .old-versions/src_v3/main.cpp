// Ignore this shit for now
#include "../include/kvdb_lib.hpp"

int main() {
    Key key;
    Val val;
    key.type = STRING;
    key.data = (void*)"User01.username";
    key.size = strlen((const char*)key.data) + 1;

    val.type = STRING;
    val.data = (void*)"John_Wood";
    val.size = strlen((const char*)val.data) + 1;

    KVDB::DB db(NEW_DB, "database/kv0001.db", 8);
    db.insert_entry(key, val);
    db.read_entry_key(key);
    db.write_header();

    return 0;
}