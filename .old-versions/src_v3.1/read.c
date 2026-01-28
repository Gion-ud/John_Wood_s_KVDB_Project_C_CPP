#include "../include/db_lib.h"
#define ENTRY_COUNT db.Header.EntryCount

const char* filenames[] = {
	"thumbnails/b91845e2c7da48a2834cccc04cc4f084.png",
	"thumbnails/c617845e279747c18978a413395b3ce2.png",
	"thumbnails/b4ea22d5c321432d99257a72a4fe7bdc.png",
	"thumbnails/fa45624110f84d88bdc0822f3f6b5125.png",
	"thumbnails/340178d28aff495d96a1ec748e53898c.png",
	"thumbnails/03dff38407a041f5b31dc6b02ed17f57.png",
	"thumbnails/67ac370b6012489d845ec6fe85257651.png",
	"thumbnails/445e6cd9f886445c8f46592977d79dc7.png",
	"thumbnails/c42549b697094ec99736f63b0608b92a.png",
	"thumbnails/a2f26fe9037b4f78a3d5a42570e223b4.png",
	"thumbnails/829b1e9e6ab446e280382247a8860cb2.png",
	"thumbnails/bd7e8c66e2364e00a1b8bdec0e6dd70a.png",
	"thumbnails/fd9b3f2f65af465a895c2b6d568c5da8.png",
	"thumbnails/571556c9a38246f58e74b18e32e81810.png",
	"thumbnails/81997792687940d783f84190ca2d2db1.png",
	"thumbnails/6b834d44e03a49039f36fa6df0a62209.png",
	"thumbnails/088e62be7918436f95cd9fea9ceb3e0f.png",
	"thumbnails/79031ef293234883b5338b1adc856d0f.png",
	"thumbnails/4d7bd4bff0e743f2a86c3a234aedfbeb.png",
	"thumbnails/2a52da170b35439e9cbd1dd80c9baad1.png",
	"thumbnails/42f4edaedac5422fa04bd1742008706a.png",
	"thumbnails/fc83760f62ba4e778c20cfc14fd9e4f4.png"
};

int main() {
    DBObject* dbp = (DBObject*)DBOpen("database/thumbnails_0001.db");
    if (!dbp) return 1;
#define db (*dbp)
    //open_file_hash_table(&db, "database/thumbnails_0001_hash_table.bin");
    //close_file_hash_table(&db);
    // exit(0);
    print_dbg_msg(ESC COLOUR_GREEN "[DEBUG] " ESC RESET_COLOUR"db.Header.EntryCount=%u\n", db.Header.EntryCount);
    Key* key = (Key*)malloc(db.Header.MaxEntryCount * sizeof(Key));
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
        ReadDBEntryKey(&db, key[i]);
        //int idx = kvdb_search_key_id(db, key[i]);
        //PRINT_DBG_MSG("idx[%d]=%d\n", i, idx);
        //if (idx < 0) continue;
        //ReadDBEntry(&db, idx);
    }

    for (int i = 0; i < ENTRY_COUNT; i++) {
        if (key[i].data) free(key[i].data);
    }

    free(key);
    CloseDB(&db);
#undef db
    return 0;
}