#include "../include/db_lib.h"
int main() {
    DBObject db = DBOpen("database/KVDB.db");
    Key* key = (Key*)alloca(4* sizeof(Key));

    key[0].size = 16;
    key[0].type = STRING; 
    key[0].data = "image.raw";
    ReadDBEntryKey(&db, key[0]);

    key[1].size = 16;
    key[1].type = STRING; 
    key[1].data = "FirstName";
    ReadDBEntryKey(&db, key[1]);

    key[2].size = 16;
    key[2].type = STRING;
    key[2].data = "LastName";
    ReadDBEntryKey(&db, key[2]);

    CloseDB(&db);
    return 0;
}