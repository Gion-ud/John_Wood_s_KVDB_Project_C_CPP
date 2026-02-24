
extern "C" {
    #include "kvdb.h"
}

int main() {
    DBObject *dbp = KVDB_DBObject_create("../database/images.db", 4);

    KVDB_DBObject_close(dbp);
    return 0;
}