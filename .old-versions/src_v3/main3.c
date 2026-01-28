#include "../include/db_lib.h"
// ..

#define ENTRY_COUNT 1049

int main() {
    //printf(ESC COLOUR_GREEN "%s\n" ESC RESET_COLOUR, filenames[0]);
    //exit(0);
    #ifdef DB_MAX_ENTRY_COUNT
        #undef DB_MAX_ENTRY_COUNT
    #endif
    #define DB_MAX_ENTRY_COUNT 1024 + 256

    DBObject db = DBInit("database/pokemon.db", DB_MAX_ENTRY_COUNT);
    init_file_hash_table(&db, "database/pokemon_hash_table.bin");
    Key key[DB_MAX_ENTRY_COUNT] = {0};
    Val val[DB_MAX_ENTRY_COUNT] = {0};
    for (uint32_t i = 0; i < ENTRY_COUNT; i++) {
        unsigned char key_data[4] = {0};
        memcpy((unsigned char*)key_data, (unsigned char*)&i, 4);
        key[i].size = 5;
        key[i].data = (unsigned char*)key_data;
        if (!key[i].data) { PRINT_DBG_MSG("malloc failed\n"); exit(1); };
        key[i].type = UNSIGNED | INT;
        PRINT_DBG_MSG(ESC COLOUR_CYAN "%.*s\n" ESC RESET_COLOUR, 4, (unsigned char*)&i);
        InsertEntry(&db, key[i], val[i]);
    }
    

    // DeleteEntry(&db, 10);
    // DeleteEntry(&db, 7);
    WriteDBHeader(&db);
    close_file_hash_table(&db);
    CloseDB(&db);

    return 0;
}