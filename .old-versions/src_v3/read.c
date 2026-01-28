#include "../include/db_lib.h"

const char* filenames[] = {
	"thumbnails/477e16d1592c4a4e9252387ef88334d0.bmp",
	"thumbnails/8998e88eac7b4e02bea30c73302a275a.bmp",
	"thumbnails/e3453b6aa95541c2bea162ecd659c2e8.bmp",
	"thumbnails/dcd79c6445fe4fe389283c08d1efd2be.bmp",
	"thumbnails/a13ac8ec11b2459aa257132bf38a23ef.bmp",
	"thumbnails/3f17f3e8889d4f078311d231f7046cb7.bmp",
	"thumbnails/9117fa64d7eb4d679ce57a25643d20a4.bmp",
	"thumbnails/7a4b4fe09d534a1aaf6b00ba5ad548ac.bmp",
	"thumbnails/e4cf208f16ed434d8a9790f1ba65bf19.bmp",
	"thumbnails/78a4d942eb4f4569bf6203a9750e7af0.bmp",
	"thumbnails/eb02cf4197754c1ba62741f8d155b310.bmp",
	"thumbnails/96c9275c82c5462c86ca034fc6b990af.bmp",
	"thumbnails/08693d8c42e749e380113efcf6432469.bmp",
	"thumbnails/fe783b26d5ae40378ebea8fac08571d0.bmp",
	"thumbnails/9e31411754c249d4baa6007ccb68229b.bmp",
	"thumbnails/68a76808ec864d7fb0848af687b2de0e.bmp",
	"thumbnails/c86274dc67424afb865f682900a38d67.bmp",
	"thumbnails/d4af5f2dc7be45e69c666f1956e04869.bmp",
	"thumbnails/71ad27cdd4474bffa877b612a4df90eb.bmp",
	"thumbnails/b13d2c2a88384c24874c27dd63716a56.bmp",
	"thumbnails/57736d5905924222985b43cc881cb571.bmp",
	"thumbnails/04e5fffd7c604c738434d73d5ccae479.bmp"
};

#define ENTRY_COUNT db.Header.EntryCount

int main() {
    DBObject db = DBOpen("database/thumbnails_0001.db");
    open_file_hash_table(&db, "database/thumbnails_0001_hash_table.mem");
    close_file_hash_table(&db);
    // exit(0);
    printf(ESC COLOUR_GREEN "[DEBUG] " ESC RESET_COLOUR"db.Header.EntryCount=%u\n\n\n", db.Header.EntryCount);
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
        long idx = search_key(db, key[i]);
        PRINT_DBG_MSG("idx[%d]=%ld\n", i, idx);
        if (idx < 0) continue;
        ReadDBEntry(&db, idx);
    }

    for (int i = 0; i < ENTRY_COUNT; i++) {
        if (key[i].data) free(key[i].data);
    }

    free(key);
    CloseDB(&db);
    return 0;
}