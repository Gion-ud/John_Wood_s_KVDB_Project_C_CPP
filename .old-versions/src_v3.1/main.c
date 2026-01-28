#include "../include/kvdb_lib.h"
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

#define ENTRY_COUNT sizeof(filenames) / sizeof(char*)

#define DB_MAX_ENTRY_COUNT 32

int main() {
    DBObject* dbp = (DBObject*)DBInit("database/thumbnails_0001.db", DB_MAX_ENTRY_COUNT);
#define db (*dbp)
    //init_file_hash_table(&db, "database/thumbnails_0001_hash_table.bin");
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
    WriteDBHeader(&db);

    //close_file_hash_table(&db);
    CloseDB(&db);
#undef db
    return 0;
}