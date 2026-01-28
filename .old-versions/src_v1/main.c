#include "../include/db_lib.h"

#define PRINT_DBG_MSG(...) fprintf(stderr, __VA_ARGS__); fflush(stderr)

int main() {
    #ifdef DB_MAX_ENTRY_COUNT
        #undef DB_MAX_ENTRY_COUNT
    #endif
    #define DB_MAX_ENTRY_COUNT 8
    PRINT_DBG_MSG("src/main.c::line10: entered main\n");

    DBObject db = DBInit("database/KVDB.db", DB_MAX_ENTRY_COUNT);
    Key key[DB_MAX_ENTRY_COUNT];
    Val val[DB_MAX_ENTRY_COUNT];
    // key.type = val.type = 0;
    {
        #ifdef KEY_SIZE
            #undef KEY_SIZE
        #endif
        #ifdef VAL_SIZE
            #undef VAL_SIZE
        #endif
        #define KEY_SIZE 32
        #define VAL_SIZE 32
        key[0].data = (char*)alloca(KEY_SIZE * sizeof(char));
        val[0].data = (char*)alloca(VAL_SIZE * sizeof(char));
        //memset(key.data, 0, KEY_SIZE);
        //memset(val.data, 0, VAL_SIZE);

        key[0].size = KEY_SIZE;
        key[0].type = STRING; 
        key[0].data = (char*)"Path: Resources/Image.png";
        //((char*)key.data)[KEY_SIZE - 1] = '\0';

        val[0].size = VAL_SIZE;
        // printf("Before: val.type=0x%08X\n", val.type);
        val[0].type = STRING;
        val[0].data = (char*)"Category: Anime Art";
        //((char*)val.data)[VAL_SIZE - 1] = '\0';

        InsertEntry(&db, key[0], val[0]);
    }

    {
        #ifdef KEY_SIZE
            #undef KEY_SIZE
        #endif
        #ifdef VAL_SIZE
            #undef VAL_SIZE
        #endif
        #define KEY_SIZE 16
        #define VAL_SIZE 48
        key[1].data = (unsigned char*)alloca(KEY_SIZE * sizeof(unsigned char));
        val[1].data = (unsigned char*)alloca(VAL_SIZE * sizeof(unsigned char));

        key[1].size = KEY_SIZE;
        key[1].type = STRING; 
        key[1].data = (char*)"image.raw";
        //((char*)key.data)[KEY_SIZE - 1] = '\0';

        val[1].size = VAL_SIZE;
        // printf("Before: val.type=0x%08X\n", val.type);
        val[1].type = RAW_BYTES;
        static unsigned char val_bytes[VAL_SIZE] = {
            0xFF, 0xFF, 0xFF,
            0x00, 0x00, 0x00,
            0x00, 0x00, 0x00,
            0xFF, 0xFF, 0xFF,

            0x00, 0x00, 0x00,
            0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF,
            0x00, 0x00, 0x00,

            0x00, 0x00, 0x00,
            0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF,
            0x00, 0x00, 0x00,

            0xFF, 0xFF, 0xFF,
            0x00, 0x00, 0x00,
            0x00, 0x00, 0x00,
            0xFF, 0xFF, 0xFF
        };
        val[1].data = (unsigned char*)val_bytes;
        //((char*)val.data)[VAL_SIZE - 1] = '\0';

        InsertEntry(&db, key[1], val[1]);
    }

    {
        #ifdef KEY_SIZE
            #undef KEY_SIZE
        #endif
        #ifdef VAL_SIZE
            #undef VAL_SIZE
        #endif
        #define KEY_SIZE 32
        #define VAL_SIZE 64
        key[2].data = (unsigned char*)alloca(KEY_SIZE * sizeof(unsigned char));
        val[2].data = (unsigned char*)alloca(VAL_SIZE * sizeof(unsigned char));
        //memset(key.data, 0, KEY_SIZE);
        //memset(val.data, 0, VAL_SIZE);

        key[2].size = KEY_SIZE;
        key[2].type = STRING; 
        key[2].data = (char*)"random_bytes.bin";
        //((char*)key.data)[KEY_SIZE - 1] = '\0';

        val[2].size = VAL_SIZE;
        // printf("Before: val.type=0x%08X\n", val.type);
        val[2].type = RAW_BYTES;
        static unsigned char val_bytes[VAL_SIZE] = {
            0x3A, 0xF1, 0x9C, 0x07, 0xD4, 0x2B, 0x8E, 0x55,
            0xA0, 0x6C, 0x11, 0xE9, 0x4F, 0x90, 0x27, 0xBD,
            0x62, 0x18, 0xC3, 0x7A, 0x0D, 0xFE, 0x34, 0xA9,
            0x5E, 0x81, 0xD0, 0x2F, 0x9B, 0x46, 0xCC, 0x73,
            0xE2, 0x05, 0x99, 0x60, 0x1A, 0xB4, 0xF8, 0x2C,
            0x4A, 0xDE, 0x87, 0x13, 0x6F, 0xC1, 0x08, 0x95,
            0xAF, 0x3D, 0x72, 0xE6, 0x10, 0x58, 0xCB, 0x24,
            0x9E, 0xF3, 0x44, 0x8A, 0xD9, 0x01, 0x6A, 0xB7
        };
        val[2].data = (unsigned char*)val_bytes;
        //((char*)val.data)[VAL_SIZE - 1] = '\0';

        InsertEntry(&db, key[2], val[2]);
    }

    {
        #ifdef KEY_SIZE
            #undef KEY_SIZE
        #endif
        #ifdef VAL_SIZE
            #undef VAL_SIZE
        #endif
        #define KEY_SIZE 4
        #define VAL_SIZE 128
        key[3].data = (unsigned char*)alloca(KEY_SIZE * sizeof(unsigned char));
        val[3].data = (unsigned char*)alloca(VAL_SIZE * sizeof(unsigned char));

        key[3].size = KEY_SIZE;
        key[3].type = INT32 | UNSIGNED;
        unsigned int key_data = 0x80800444;
        unsigned char *key_bytes = (unsigned char*)alloca(KEY_SIZE);
        memcpy(key_bytes, (unsigned char*)&key_data, KEY_SIZE);
        key[3].data = (unsigned char*)key_bytes;

        val[3].size = VAL_SIZE;
        val[3].type = RAW_BYTES;
        static unsigned char val_bytes[VAL_SIZE] = {
            0x7A,0x1F,0xC8,0x34,0x9B,0xE2,0x56,0x0D,0xA4,0x8F,0x12,0xBD,0x63,0xF9,0x20,0x5E,\
            0x91,0x3C,0x6A,0xD7,0x0B,0xE5,0x4F,0x88,0x2D,0xB2,0x7F,0x14,0xC1,0x5A,0x9E,0x03,\
            0xDE,0x27,0x8B,0x41,0xF4,0x6C,0x19,0xA0,0x53,0xBE,0x0E,0x97,0x2F,0xC6,0x71,0x48,\
            0xB9,0x05,0xDA,0x36,0xF1,0x60,0x8C,0x23,0x9F,0x4A,0xE8,0x12,0x75,0xBD,0x0C,0x93,\
            0x1A,0xF7,0x64,0xCB,0x08,0x9D,0x32,0xE5,0x56,0xAF,0x03,0xC8,0x7E,0x21,0xB0,0x4D,\
            0x6F,0x18,0xD3,0x9A,0x42,0xF5,0x0B,0x87,0xCE,0x31,0x5C,0xA6,0x0F,0xD9,0x74,0x2B,\
            0x81,0x54,0xE1,0x0A,0xC3,0x6D,0x98,0x2F,0xB6,0x43,0xFD,0x10,0x7C,0x25,0xAA,0x3E,\
            0x59,0xB8,0x06,0xD0,0x47,0x9F,0x2C,0xE7,0x11,0x65,0xBA,0x0D,0xF2,0x38,0x8E,0x51
        };
        val[3].data = (unsigned char*)val_bytes;
        //((char*)val.data)[VAL_SIZE - 1] = '\0';

        InsertEntry(&db, key[3], val[3]);
    }

    {
        #ifdef KEY_SIZE
            #undef KEY_SIZE
        #endif
        #ifdef VAL_SIZE
            #undef VAL_SIZE
        #endif
        #define KEY_SIZE 4
        #define VAL_SIZE 128
        key[4].data = (unsigned char*)alloca(KEY_SIZE * sizeof(unsigned char));
        val[4].data = (unsigned char*)alloca(VAL_SIZE * sizeof(unsigned char));

        key[4].size = KEY_SIZE;
        key[4].type = INT32 | UNSIGNED;
        unsigned int key_data = 0x8EA00E44;
        unsigned char *key_bytes = (unsigned char*)alloca(KEY_SIZE);
        memcpy(key_bytes, (unsigned char*)&key_data, KEY_SIZE);
        key[4].data = (unsigned char*)key_bytes;

        val[4].size = VAL_SIZE;
        val[4].type = RAW_BYTES;
        static unsigned char val_bytes[VAL_SIZE] = {
            0x7A,0x1F,0xC8,0x34,0x9B,0xE2,0x56,0x0D,0xA4,0x8F,0x12,0xBD,0x63,0xF9,0x20,0x5E,\
            0x91,0x3C,0x6A,0xD7,0x0B,0xE5,0x4F,0x88,0x2D,0xB2,0x7F,0x14,0xC1,0x5A,0x9E,0x03,\
            0xDE,0x27,0x8B,0x41,0xF4,0x6C,0x19,0xA0,0x53,0xBE,0x0E,0x97,0x2F,0xC6,0x71,0x48,\
            0xB9,0x05,0xDA,0x36,0xF1,0x60,0x8C,0x23,0x9F,0x4A,0xE8,0x12,0x75,0xBD,0x0C,0x93,\
            0x1A,0xF7,0x64,0xCB,0x08,0x9D,0x32,0xE5,0x56,0xAF,0x03,0xC8,0x7E,0x21,0xB0,0x4D,\
            0x6F,0x18,0xD3,0x9A,0x42,0xF5,0x0B,0x87,0xCE,0x31,0x5C,0xA6,0x0F,0xD9,0x74,0x2B,\
            0x81,0x54,0xE1,0x0A,0xC3,0x6D,0x98,0x2F,0xB6,0x43,0xFD,0x10,0x7C,0x25,0xAA,0x3E,\
            0x59,0xB8,0x06,0xD0,0x47,0x9F,0x2C,0xE7,0x11,0x65,0xBA,0x0D,0xF2,0x38,0x8E,0x51
        };
        val[4].data = (unsigned char*)val_bytes;
        InsertEntry(&db, key[4], val[4]);
    }

    {
        #ifdef KEY_SIZE
            #undef KEY_SIZE
        #endif
        #ifdef VAL_SIZE
            #undef VAL_SIZE
        #endif
        #define KEY_SIZE 16
        #define VAL_SIZE 32
        key[5].data = (char*)alloca(KEY_SIZE * sizeof(char));
        val[5].data = (char*)alloca(VAL_SIZE * sizeof(char));

        key[5].size = KEY_SIZE;
        key[5].type = STRING;
        key[5].data = "FirstName";

        val[5].size = VAL_SIZE;
        val[5].type = STRING;
        val[5].data = "John";
        InsertEntry(&db, key[5], val[5]);
    }

    {
        #ifdef KEY_SIZE
            #undef KEY_SIZE
        #endif
        #ifdef VAL_SIZE
            #undef VAL_SIZE
        #endif
        #define KEY_SIZE 16
        #define VAL_SIZE 32
        key[6].data = (char*)alloca(KEY_SIZE * sizeof(char));
        val[6].data = (char*)alloca(VAL_SIZE * sizeof(char));

        key[6].size = KEY_SIZE;
        key[6].type = STRING;
        key[6].data = "LastName";

        val[6].size = VAL_SIZE;
        val[6].type = STRING;
        val[6].data = "Wood";
        InsertEntry(&db, key[6], val[6]);
    }

    {
        #ifdef KEY_SIZE
            #undef KEY_SIZE
        #endif
        #ifdef VAL_SIZE
            #undef VAL_SIZE
        #endif
        #define KEY_SIZE 16
        #define VAL_SIZE 12
        key[7].data = (char*)alloca(KEY_SIZE * sizeof(char));
        val[7].data = (char*)alloca(VAL_SIZE * sizeof(char));

        key[6].size = KEY_SIZE;
        key[6].type = STRING;
        key[6].data = "DateOfBirth";

        val[6].size = VAL_SIZE;
        val[6].type = STRING;
        val[6].data = "04/04/2009";
        InsertEntry(&db, key[6], val[6]);
    }

    //ReadDBEntry(&db, 0);
    //ReadDBEntry(&db, 1);
    //ReadDBEntry(&db, 2);
    //ReadDBEntry(&db, 3);

    DeleteEntry(&db, 3);

    ReadDBEntry(&db, 0);
    ReadDBEntry(&db, 1);
    ReadDBEntry(&db, 2);
    ReadDBEntry(&db, 3);
    ReadDBEntry(&db, 4);
    ReadDBEntry(&db, 5);
    ReadDBEntry(&db, 6);
    ReadDBEntry(&db, 7);
    ReadDBEntry(&db, 8);

    #ifdef KEY_SIZE
        #undef KEY_SIZE
    #endif
    #ifdef VAL_SIZE
        #undef VAL_SIZE
    #endif

    WriteDBHeader(&db);
    //CommitDB(&db);
    CloseDB(&db);

    return 0;
}