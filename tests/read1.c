#include "db_lib.h"
#define ENTRY_COUNT db.Header.EntryCount

const char* filenames[] = {
	"thumbnails/294947af87b9422da85088f5efe847b0.png",
	"thumbnails/b54b286513b049b5a0540d7c99f71869.png",
	"thumbnails/2c927876b766440189ce9c0bb242ebb7.png",
	"thumbnails/7b4572afbb5d4eb58a1c7a644b630361.png",
	"thumbnails/2686075ee24c487597bf6865acd1b05b.png",
	"thumbnails/76a0bd570b6347cc8c46d6203c361fbe.png",
	"thumbnails/632c0e751f964bbe9d7c1a76fba65563.png",
	"thumbnails/fde63d9494124de8a3a90ac5863286c5.png",
	"thumbnails/eefa842102594db8b256e91ed57ea4b1.png",
	"thumbnails/82c6437294ae4c1f93e37c0717fa5146.png",
	"thumbnails/fad1e919bf7d4befab3fd8414093d61e.png",
	"thumbnails/a4f8d11ee1dd4ef29da43a653b1b0eef.png",
	"thumbnails/a42094b9a04f4f649c0ef8686cf38172.png",
	"thumbnails/958be67bba874a70ad8dd8863c4ec8e5.png",
	"thumbnails/8a0049db259046b093ee0ac48bbe32be.png",
	"thumbnails/42b9b66eb4ed466aa74173acd48a4cc2.png",
	"thumbnails/def9b836238b41b5899a3a7d6aaf11d3.png",
	"thumbnails/d392c096d0b949a19556a20ba5c796ff.png",
	"thumbnails/87517b634215494aad1578a421538335.png",
	"thumbnails/699f54d462eb40279602782d6c4a4801.png",
	"thumbnails/c1df7ce7e1fc42b7b32c867c719cbcef.png",
	"thumbnails/5b817ed93618449a89efc2c6a7025d34.png"
};

int main() {
    DBObject* dbp = (DBObject*)DBOpen("database/thumbnails_0001.db");
    if (!dbp) return 1;
#define db (*dbp)
    //open_file_hash_table(&db, "database/thumbnails_0001_hash_table.bin");
    //close_file_hash_table(&db);
    // exit(0);


    Key* key = (Key*)malloc(db.Header.EntryCapacity * sizeof(Key));
    if (!key) { PRINT_DBG_MSG(ESC COLOUR_RED); perror("malloc"); PRINT_DBG_MSG(ESC RESET_COLOUR); exit(1); }
    for (int i = 0; i < ENTRY_COUNT; i++) {
        PRINT_DBG_MSG(ESC COLOUR_BLUE "filenames[%d]: \"%s\"\n" ESC RESET_COLOUR, i, filenames[i]);
        key[i].type = STRING;
        key[i].size = strlen(filenames[i]) + 1;
        key[i].data = (char*)malloc(key[i].size);
        if (!key[i].data) {
            PRINT_DBG_MSG(ESC COLOUR_RED); perror("malloc"); PRINT_DBG_MSG(ESC RESET_COLOUR);
            continue;
        }
        memcpy(key[i].data, filenames[i], key[i].size);
    }

    // for (int i = 0; i < ENTRY_COUNT; i++) ReadDBEntry(&db, i);
    for (int i = 0; i < ENTRY_COUNT; i++) { 
        // ReadDBEntryKey(&db, key[i]);
        KVPair *kv = ReadDBEntry(&db, i);
        if (!kv) continue;
        PrintKvPair(kv);
        DestroyKVPair(kv);
        //int idx = kvdb_search_key_id(db, key[i]);
        //PRINT_DBG_MSG("idx[%d]=%d\n", i, idx);
        //if (idx < 0) continue;
        //ReadDBEntry(&db, idx);
    }

    /*

    DeleteEntry(&db, 1);
    DeleteEntry(&db, 4);
    DeleteEntry(&db, 3);
    DeleteEntry(&db, 7);
    DeleteEntry(&db, 16);
    DeleteEntry(&db, 11);
    DeleteEntry(&db, 10);
    DeleteEntry(&db, 15);
    DeleteEntry(&db, 21);
    DeleteEntry(&db, 32);
    for (int i = 0; i < 32; i++) { 
        // ReadDBEntryKey(&db, key[i]);
        ReadDBEntryKey(&db, key[i]);
        //int idx = kvdb_search_key_id(db, key[i]);
        //PRINT_DBG_MSG("idx[%d]=%d\n", i, idx);
        //if (idx < 0) continue;
        //ReadDBEntry(&db, idx);
    }

    CloseDB(&db);

    dbp = (DBObject*)DBOpen("database/thumbnails_0001.db");
    for (int i = 0; i < 32; i++) { 
        // ReadDBEntryKey(&db, key[i]);
        ReadDBEntryKey(&db, key[i]);
        //int idx = kvdb_search_key_id(db, key[i]);
        //PRINT_DBG_MSG("idx[%d]=%d\n", i, idx);
        //if (idx < 0) continue;
        //ReadDBEntry(&db, idx);
    }

    */


    for (int i = 0; i < ENTRY_COUNT; i++) {
        if (key[i].data) free(key[i].data);
    }

    free(key);
    CloseDB(&db);

#undef db
    return 0;
}