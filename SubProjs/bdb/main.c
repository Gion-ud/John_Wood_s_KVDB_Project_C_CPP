#include <db.h>
#include <stdio.h>
#include <string.h>

int main() {
    DB *dbp;
    int ret;

    // Create database handle
    ret = db_create(&dbp, NULL, 0);
    if (ret != 0) {
        fprintf(stderr, "db_create failed: %s\n", db_strerror(ret));
        return 1;
    }

    // Open the database (Btree format)
    ret = dbp->open(dbp, NULL, "kv_dat_bdb.db", NULL, DB_BTREE, DB_CREATE, 0);
    if (ret != 0) {
        fprintf(stderr, "DB::open failed: %s\n", db_strerror(ret));
        return 1;
    }

    DBT key, val;
    memset(&key, 0, sizeof(DBT));
    memset(&val, 0, sizeof(DBT));

    const char *k = "hello";
    const char *v = "world";
    key.data = (void*)k;
    key.size = strlen(k) + 1;
    val.data = (void*)v;
    val.size = strlen(v) + 1;

    // Store in database
    ret = dbp->put(dbp, NULL, &key, &val, 0);
    if (ret != 0) {
        fprintf(stderr, "DB::put failed: %s\n", db_strerror(ret));
        return 1;
    }

    // Retrieve from database
    DBT result = {0};
    ret = dbp->get(dbp, NULL, &key, &result, 0);
    if (ret == 0) {
        printf("Retrieved value: %s\n", (char*)result.data);
    }

    // Close database
    dbp->close(dbp, 0);

    return 0;
}
