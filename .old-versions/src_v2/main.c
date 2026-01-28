#include "../include/db_lib.h"
// ..
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

#define ENTRY_COUNT 10

int main() {
    //printf(ESC COLOUR_GREEN "%s\n" ESC RESET_COLOUR, filenames[0]);
    //exit(0);
    #ifdef DB_MAX_ENTRY_COUNT
        #undef DB_MAX_ENTRY_COUNT
    #endif
    #define DB_MAX_ENTRY_COUNT 16

    DBObject db = DBInit("database/KVDB.db", DB_MAX_ENTRY_COUNT);
    Key key[DB_MAX_ENTRY_COUNT] = {0};
    Val val[DB_MAX_ENTRY_COUNT] = {0};
    for (int i = 0; i < ENTRY_COUNT; i++) {
        key[i].size = strlen(filenames[i]) + 1;
        key[i].data = (char*)alloca(key[i].size);
        key[i].data = (void*)filenames[i];
        key[i].type = STRING;

        FILE* fp = fopen(filenames[i], "rb");
        if (!fp) { perror("fopen"); continue; }

        fseek(fp, 0, SEEK_END);
        val[i].size = (uint64_t)ftell(fp);
        PRINT_DBG_MSG(ESC COLOUR_YELLOW "val[i].size=%llu\n" ESC RESET_COLOUR, (unsigned long long)val[i].size);
        fseek(fp, 0, SEEK_SET);
        val[i].type = BLOB;
        val[i].data = (unsigned char*)malloc(val[i].size);
        if (!val[i].data) { perror("malloc"); fclose(fp); continue; }
        fread((unsigned char*)val[i].data, val[i].size, 1, fp);
        InsertEntry(&db, key[i], val[i]);
        free(val[i].data);
        fclose(fp);
    }

    DeleteEntry(&db, 4);

    WriteDBHeader(&db);
    //CommitDB(&db);
    CloseDB(&db);

    return 0;
}