#include "../include/db_lib.h"
#define ENTRY_COUNT db.Header.EntryCount

const char* filenames[] = {
	"thumbnails/72f1a3b2bd91465f9b428a150606c7c7.bmp",
	"thumbnails/6c9e93461b0a4f248a512695a96e6b0b.bmp",
	"thumbnails/a2cbc3acb5e34d5a96352907d72caf5f.bmp",
	"thumbnails/0e98a2bee1dc4d37b4d48f38fa23f719.bmp",
	"thumbnails/da01f0c55acf4742a5b8611bdc09bbcc.bmp",
	"thumbnails/36a00766d039489fad82dec3ea0742b9.bmp",
	"thumbnails/cad70e9e018144cb8768e6204c03f226.bmp",
	"thumbnails/f18004c14bcf430d846391276518107d.bmp",
	"thumbnails/1e7547ec0e014a0093a1aa8288025526.bmp",
	"thumbnails/ee123480bea546bc9628b31ccad99067.bmp"
};

int main() {
    DBObject db = DBOpen("database/KVDB.db");
    Key* key = (Key*)malloc(ENTRY_COUNT * sizeof(Key));
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
    for (int i = 0; i < ENTRY_COUNT; i++) ReadDBEntryKey(&db, key[i]);


    for (int i = 0; i < ENTRY_COUNT; i++) {
        if (key[i].data) free(key[i].data);
    }
    free(key);
    CloseDB(&db);
    return 0;
}