#include "db_utils.h"
#include "db_lib.h"
#include "hash_table_lib.h"

#define DB (*dbp)
#define STACK_MEM_SIZE_MAX 1024u * 1024u

static const ubyte_t DBFileMagic[MAGIC_SIZE] = { 'K', 'V', 'D', 'B', '\r', '\n', ' ', '\0' };
static const ubyte_t DBEOFMagic[MAGIC_SIZE] = { '\n', '.', 'D', 'B', 'E', 'O', 'F', '\0' };

static const ubyte_t pad_arr[8] = {0};

// # Below are helper functions
static inline int find_str_nul(char* buffer, size_t buf_size) {
//  This function checks if a string is NUL terminated;
    for (size_t i = 0; i < buf_size; i++) {
        if (buffer[i] == '\0') {
            return i; // returns idx of '\0'(effectively strlen) when NUL is read 
        }
    }
    return -1; // NUL not found within buf_size
}

static inline uint64_t GetDataEntrySize(uint32_t KeySize, uint64_t ValSize) {
    return (uint64_t) sizeof(DataEntryHeader) + KeySize + ValSize;
}
static inline uint64_t GetIndexEntryOffset(DBObject db, uint32_t EntryID) {
    return (uint64_t) db.Header.IndexTableOffset + EntryID * db.Header.IndexEntrySize;
}

char* conv_bytes_hex(const unsigned char* bytes, size_t size) {
//  This function converts raw bytes into str "0x??,0x??,0x??,...,0x??"
    char* buffer = (char*)malloc(size * 8ull);
    if (!buffer) {
        perror("malloc failed");
        return NULL;
    }
    buffer[0] = '\0';
    char ByteHex[8];
    int len;
    for (int i = 0; i < size; i++) {
        sprintf(ByteHex, "0x%02X", (unsigned char)bytes[i]);
        if ((i + 1) % 16 != 0 && i < size - 1) {
            // 16 bytes in a row
            len = strlen(ByteHex);
            ByteHex[len++] = ',';
            ByteHex[len] = '\0';
        } else {
            if (i < size - 1) {
                len = strlen(ByteHex);
                ByteHex[len++] = ',';
                ByteHex[len] = '\0';

                len = strlen(ByteHex);
                ByteHex[len++] = '\\'; // '\\' used to cancel out LF
                ByteHex[len] = '\0';
            }

            len = strlen(ByteHex);
            ByteHex[len++] = '\n';
            ByteHex[len] = '\0';
        }
        strcat(buffer, ByteHex);
    }

    return (char*)buffer;
    // needs to be freed!
}
static inline void append_str_hex(char** dest, unsigned char* src, size_t src_size) {
    char* tmp = (char*)conv_bytes_hex(src, src_size);
    strcat(*dest, tmp);
    free(tmp);
}

static inline long get_db_size(DBObject db) {
    fseek(db.fp, 0, SEEK_END);
    long db_size = (long)ftell(db.fp);
    fseek(db.fp, db.OffsetPtr, SEEK_SET);
    return db_size;
}
// static inline void print_hash_table(DBObject db);

// # Below is an FNV hash function
#define FNV_OFFSET_BASIS    14695981039346656037ULL
#define FNV_PRIME           1099511628211ULL
#define hash_str(key) kvdb_hash(key, strlen(key) + 1)
hash_t kvdb_hash(const unsigned char* key, size_t len) {
    hash_t hash_val = FNV_OFFSET_BASIS;
    for (size_t i = 0; i < len; i++) {
        hash_val ^= key[i]; // XOR hash_val with current key byte
        hash_val *= FNV_PRIME; // Multiply hash_val with FNV_PRIME
    }
    return hash_val;
}
#undef HASH_INIT_NUM
#undef HASH_MUL_NUM


long int kvdb_search_key(DBObject db, Key key) { // old
#define HASH_TABLE_SIZE db.Header.EntryCapacity
    hash_t h = kvdb_hash((unsigned char*)key.data, (uint32_t)key.size);
    size_t idx = h % HASH_TABLE_SIZE;
    for (int i = 0; i < db.Header.EntryCapacity; i++) {
        if (db.HashTable[i] == h) {
            // printf("Matching\n");
            // Matching hash table idx
            if (key.size != db.key_arr[i].size) continue;
            if (key.type != db.key_arr[i].type) continue;
            //if (memcmp(&db.key_arr[i], &key, sizeof(Key)) == 0) {
            if (memcmp(db.key_arr[i].data, key.data, db.key_arr[i].size) == 0) {
                return i;
            }
        }
    }
    return -1;  // key not found
#undef HASH_TABLE_SIZE 
}

int kvdb_search_key_id(DBObject* dbp, Key key) {
#define db (*dbp)
#define HASH_TABLE_SIZE db.Header.EntryCapacity
    hash_t idx = search_key(db.htObj, key.data, key.size);
    if (idx < 0) return -1;
    return db.EntryID_arr_htidx[idx];  // key not found
#undef HASH_TABLE_SIZE
#undef db 
}

// # no more helper functions below


void CloseDB(DBObject* dbp);
DBObject* DBInit(const char* filepath, uint32_t EntryCapacity) {
//  # This function creates a new db object\
    # DBInit is used to create a new on disk db and DBOpen is used to open an existing db\
    # Init , open and close for Hash table on disk IO are done by separate functions
    if (EntryCapacity > INT32_MAX) { print_err_msg("EntryCapacity:i32 overflow\n"); return NULL; };
    DBObject* dbp = (DBObject*)calloc(1, sizeof(DBObject));
    if (!dbp) { print_err_msg("(DBObject*)calloc(1, sizeof(DBObject)) failed\n"); goto DBInit_failed_cleanup; }
    memset(dbp, 0, sizeof(DBObject));
#define db (*dbp)
    db.filepath = (char*)malloc(FILE_PATH_SIZE);
    if (!db.filepath) {
        print_err_msg("(char*)malloc(FILE_PATH_SIZE) failed\n"); 
        goto DBInit_failed_cleanup;
    }
    memcpy((char*)db.filepath, filepath, FILE_PATH_SIZE); // \
    # This stores file path string to DBObject;\
    # needs to be freed when the obj gets destroyed

    db.fp = fopen(db.filepath, "wb+");
    // This opens on disk db file
    if (!db.fp) { print_err_msg("fopen failed\n"); CloseDB(&db); return NULL; }

    // Write known metadata to header
    memcpy((ubyte_t*)db.Header.Magic, DBFileMagic, MAGIC_SIZE);
    db.Header.Version = VERSION;
    db.Header.ByteOrder = LE;
    db.Header.HeaderSize = HEADER_SIZE;
    db.Header.IndexEntrySize = INDEX_ENTRY_SIZE;
    db.Header.IndexTableOffset = HEADER_SIZE;   // PlaceHolder
    db.Header.EntryCount = 0;   // Also used as counter/ID for entry count
    db.Header.ValidEntryCount = 0;
    db.Header.EntryCapacity = EntryCapacity;
    db.Header.DataSectionOffset = HEADER_SIZE;
    db.Header.DataEntryHeaderSize = DATA_ENTRY_HEADER_SIZE;
    //memcpy(db.Header._pad0, (ubyte_t*)pad_arr, sizeof(db.Header._pad0));
    //memcpy(db.Header._pad1, (ubyte_t*)pad_arr, sizeof(db.Header._pad1));

    //PrintHeader(stderr, db.Header);

    db.IndexTable = (DBIndexEntry*)calloc(EntryCapacity, INDEX_ENTRY_SIZE); //\
    # This line allocates the IndexTable in db obj;\
    # it will be eventually written to disk
    if (!db.IndexTable) {
        print_err_msg("(DBIndexEntry*)calloc(EntryCapacity * INDEX_ENTRY_SIZE) failed\n");
        goto DBInit_failed_cleanup;
    }

    db.key_arr = (Key*)calloc(EntryCapacity, sizeof(Key));
    if (!db.key_arr) {
        print_err_msg("(Key*)calloc(EntryCapacity, sizeof(Key)) failed\n");
        goto DBInit_failed_cleanup;
    }

    db.EntryID_arr_htidx = (int*)calloc(db.Header.EntryCapacity, sizeof(int));
    if (!db.EntryID_arr_htidx) {
        print_err_msg("(int*)calloc(db.Header.EntryCapacity, sizeof(int)) failed\n");
        goto DBInit_failed_cleanup;
    }
    for (uint32_t i = 0; i < db.Header.EntryCapacity; i++) {
        db.EntryID_arr_htidx[i] = -1;
    }

    db.HashTable = NULL;

    db.htObj = (HTObject*)malloc(sizeof(HTObject));
    if (!db.htObj) {
        print_err_msg("(HashTableObject*)malloc(sizeof(HashTableObject)) failed\n");
        goto DBInit_failed_cleanup;
    }
    int ret = InitHashTableObject(db.htObj, db.Header.EntryCapacity);
    if (ret < 0) goto DBInit_failed_cleanup;

    db.OffsetPtr = db.Header.DataSectionOffset;
    PRINT_DBG_MSG(ESC COLOUR_YELLOW"\ndb.fp.EOF_Offset=%ld\n" ESC RESET_COLOUR, get_db_size(db));
    return dbp;

DBInit_failed_cleanup:
    if (dbp) CloseDB(&db); return NULL;
#undef db
}

DBObject* DBOpen(const char* filepath) {
//  This function opens an existing on disk db file and creates db object
    DBObject* dbp = (DBObject*)malloc(sizeof(DBObject));
    if (!dbp) { print_err_msg("(DBObject*)malloc(sizeof(DBObject)) failed\n"); goto DBopen_failed_cleanup; }
    memset(dbp, 0, sizeof(DBObject));
#define db (*dbp)
    db.filepath = (char*)malloc(FILE_PATH_SIZE);
    if (!db.filepath) { perror("malloc(FILE_PATH_SIZE)"); goto DBopen_failed_cleanup; }
    memcpy((char*)db.filepath, filepath, FILE_PATH_SIZE);

    db.fp = fopen(db.filepath, "rb+");
    if (!db.fp) {
        perror("fopen(db.filepath, \"rb+\")");
        goto DBopen_failed_cleanup;
    }

    rewind(db.fp);
    size_t fread_cnt;
    fread_cnt = fread(&db.Header, HEADER_SIZE, 1, db.fp);
    if (fread_cnt != 1) {
        print_err_msg("fread(&db.Header, HEADER_SIZE, 1, db.fp) != 1\n");
        goto DBopen_failed_cleanup;
    }

    if (memcmp(db.Header.Magic, DBFileMagic, MAGIC_SIZE) != 0) {
        print_err_msg(ESC COLOUR_RED "Fatal Error: Invalid DB FILE\n" ESC RESET_COLOUR);
        goto DBopen_failed_cleanup;
    }
    //PrintHeader(stdout, db.Header);


    db.IndexTable = (DBIndexEntry*)calloc(db.Header.EntryCapacity, INDEX_ENTRY_SIZE);
    if (!db.IndexTable) { perror("(DBIndexEntry*)calloc(EntryCapacity * INDEX_ENTRY_SIZE)"); goto DBopen_failed_cleanup; }
    fseek(db.fp, db.Header.IndexTableOffset, SEEK_SET);
    for (int i = 0; i < db.Header.EntryCount; i++) {
        fread_cnt = fread(&db.IndexTable[i], INDEX_ENTRY_SIZE, 1, db.fp);
        if (fread_cnt != 1) {
            print_err_msg("fread(&db.IndexTable[i], INDEX_ENTRY_SIZE, 1, db.fp) != 1\n");
            goto DBopen_failed_cleanup;
        }
    }
    //PrintIndexTable(stdout, db);

    db.key_arr = (Key*)calloc(db.Header.EntryCapacity, sizeof(Key));
    if (!db.key_arr) {
        print_err_msg("(Key*)calloc(EntryCapacity, sizeof(Key)) failed\n");
        goto DBopen_failed_cleanup;
    }
    db.EntryID_arr_htidx = (int*)calloc(db.Header.EntryCapacity, sizeof(int));
    if (!db.EntryID_arr_htidx) {
        print_err_msg("(int*)calloc(db.Header.EntryCapacity, sizeof(int)) failed\n");
        goto DBopen_failed_cleanup;
    }
    for (uint32_t i = 0; i < db.Header.EntryCapacity; i++) {
        db.EntryID_arr_htidx[i] = -1;
    }

    fseek(db.fp, db.Header.DataSectionOffset, SEEK_SET);
    DataEntryHeader RecHeader;
    for (uint32_t i = 0; i < db.Header.EntryCount; i++) {
        fread_cnt = fread(&RecHeader, sizeof(DataEntryHeader), 1, db.fp);
        if (fread_cnt != 1) {
            print_err_msg("fread(&RecHeader, sizeof(DataEntryHeader), 1, db.fp) != 1\n");
            goto DBopen_failed_cleanup;
        }

        db.key_arr[i].data = (char*)malloc(RecHeader.KeySize);
        if (!db.key_arr[i].data) { PRINT_DBG_MSG("(char*)malloc(RecHeader.KeySize) failed\n"); goto DBopen_failed_cleanup; }

        fseek(db.fp, RecHeader.KeyOffset, SEEK_SET);
        fread_cnt = fread(db.key_arr[i].data, RecHeader.KeySize, 1, db.fp);
        if (fread_cnt != 1) {
            print_err_msg("fread(db.key_arr[i].data, RecHeader.KeySize, 1, db.fp) != 1\n");
            goto DBopen_failed_cleanup;
        }

        db.key_arr[i].size = RecHeader.KeySize;
        db.key_arr[i].type = RecHeader.KeyType;
        print_dbg_msg(
            "db.key_arr[%u].data=`%.*s`\n"
            "db.key_arr[%u].size=%u\n"
            "db.key_arr[%u].type=%u\n\n",
            i, db.key_arr[i].size, (unsigned char*)db.key_arr[i].data,
            i, db.key_arr[i].size,
            i, db.key_arr[i].type
        );
        fseek(db.fp, RecHeader.ValSize, SEEK_CUR);
    }

    db.htObj = (HTObject*)malloc(sizeof(HTObject));
    if (!db.htObj) {
        print_err_msg("(HashTableObject*)malloc(sizeof(HashTableObject)) failed\n");
        goto DBopen_failed_cleanup;
    }
    int ret = InitHashTableObject(db.htObj, db.Header.EntryCapacity);
    if (ret < 0) goto DBopen_failed_cleanup;
    for (uint32_t i = 0; i < db.Header.EntryCount; i++) {
        ret = insert_entry(db.htObj, (ubyte_t*)db.key_arr[i].data, (keysize_t)db.key_arr[i].size);
        db.EntryID_arr_htidx[ret] = i;
        if (ret < 0) { print_err_msg("insert_entry failed\n"); continue; }
    }

    db.OffsetPtr = db.Header.DataSectionOffset;
    PRINT_DBG_MSG(ESC COLOUR_YELLOW "\ndb.fp.EOF_Offset=%ld\n" ESC RESET_COLOUR, get_db_size(db));

    return dbp;
DBopen_failed_cleanup:
    if (dbp) CloseDB(&db); return NULL;
#undef db
}

void WriteDBHeader(DBObject* dbp);
void WriteDBIndexTable(DBObject* dbp);
void WriteDBEOFHeader(DBObject* dbp);
void CloseDB(DBObject* dbp) {
    PRINT_DBG_MSG("CloseDB(%p);\n", (void*)dbp);

    if (DB.db_modified) { WriteDBIndexTable(&DB); WriteDBEOFHeader(&DB); WriteDBHeader(&DB); }

    for (uint32_t i = 0; i < DB.Header.EntryCapacity; i++) {
        if (DB.key_arr[i].data) { free(DB.key_arr[i].data); DB.key_arr[i].data = NULL; }
    }
    if (DB.key_arr) { free(DB.key_arr); DB.key_arr = NULL; }
    if (DB.htObj) {
        DestroyHashTableObject(DB.htObj);
        free(DB.htObj);
        DB.htObj = NULL;
    }
    //if (DB.HashTable) { free(DB.HashTable); DB.HashTable = NULL; }

    if (DB.EntryID_arr_htidx) { free(DB.EntryID_arr_htidx); DB.EntryID_arr_htidx = NULL; }
    if (DB.IndexTable) { free(DB.IndexTable); DB.IndexTable = NULL; }
    if (DB.filepath) { free(DB.filepath); DB.filepath = NULL; }
    if (DB.fp) { fclose(DB.fp); DB.fp = NULL; }

    PrintDBFileHeader(stdout, dbp);
    free(dbp);
    dbp = NULL;
}


/*
    if (i >= DB.Header.EntryCapacity) {
        ulong_t newCap = DB.Header.EntryCapacity + INIT_INDEX_TABLE_CAP;
        DBIndexEntry *DB_IndexTable_tmpPtr = (DBIndexEntry*)realloc(DB.IndexTable, INDEX_ENTRY_SIZE * newCap);
        if (!DB_IndexTable_tmpPtr) {
            print_err_msg("(DBIndexEntry*)realloc(newCap, INDEX_ENTRY_SIZE * newCap) failed\n");
            return -1;
        }
        DB.IndexTable = DB_IndexTable_tmpPtr;
        Key *DB_key_arr_tmpPtr = (Key*)realloc(DB.key_arr, sizeof(key) * newCap);
        if (!DB_key_arr_tmpPtr) {
            print_err_msg("(Key*)realloc(DB.key_arr, sizeof(key) * newCap) failed\n");
            return -1;
        }
        DB.key_arr = DB_key_arr_tmpPtr;
        //print_err_msg(ESC COLOUR_RED "Error: DB full\n" ESC RESET_COLOUR);
        //return -1;
        DB.Header.EntryCapacity = newCap;
    }

*/

int InsertEntry(DBObject* dbp, Key key, Val val) {
#define i (uint32_t)DB.Header.EntryCount
    PRINT_DBG_MSG(ESC COLOUR_YELLOW "i=%u;max=%u\n" ESC RESET_COLOUR, i, DB.Header.EntryCapacity);

    if (i >= DB.Header.EntryCapacity) {
        print_err_msg(ESC COLOUR_RED "Error: DB full\n" ESC RESET_COLOUR);
        return -1;
    }
    if (key.size > MAX_KEY_SIZE) {
        print_err_msg(ESC COLOUR_RED "Error: key.size exceeds MAX_KEY_SIZE:%u\n" ESC RESET_COLOUR, MAX_KEY_SIZE);
        return -1;
    }

    DataEntryHeader RecordHeader = {0};
    RecordHeader.KeySize = key.size;
    RecordHeader.KeyType = key.type;
    RecordHeader.KeyOffset = DB.OffsetPtr + DB.Header.DataEntryHeaderSize;
    RecordHeader.ValSize = val.size;
    RecordHeader.ValType = val.type;
    RecordHeader.ValOffset = DB.OffsetPtr + DB.Header.DataEntryHeaderSize + key.size;

    fseek(DB.fp, DB.OffsetPtr, SEEK_SET);

    // # PrintDbgMsg
    PRINT_DBG_MSG(ESC COLOUR_YELLOW "\nInserting Entry: Entry[%u]\n" ESC RESET_COLOUR, i);
    PRINT_DBG_MSG(ESC COLOUR_YELLOW "CurrentOffset=%llu\n" ESC RESET_COLOUR, (unsigned long long)DB.OffsetPtr);
    // # EndPrintDbgMsg

    fwrite((DataEntryHeader*)&RecordHeader, sizeof(RecordHeader), 1, (FILE*)DB.fp);

    if (key.type == STRING) {
        char *key_data = (char*)malloc(key.size);
        if (!key_data) { perror("malloc failed"); return -1; }

        memcpy((char*)key_data, (char*)key.data, key.size);
        size_t len;
        if (find_str_nul(key_data, key.size) >= 0) {
            len = strlen(key_data);
        } else {
            len = key.size - 1; // force nul termination
        }
        memset(key_data + len, 0, key.size - len);
        fwrite((unsigned char*)key_data, 1, (size_t)key.size, (FILE*)DB.fp);

        free(key_data);
    } else {
        // DBGPrint
        //printf("%d\n", *(int*)key.data);
        fwrite((unsigned char*)key.data, 1, (size_t)key.size, (FILE*)DB.fp);
    }

    /*
    if (DB.HashTable != NULL) {
        DB.HashTable[i] = kvdb_hash(key.data, key.size);
        printf("DB.HashTable[%.4u]=0x%.16llx;idx=%.4llu\n", i, DB.HashTable[i], DB.HashTable[i] % DB.Header.EntryCapacity);
        fwrite(&DB.HashTable[i], HASH_SIZE, 1, DB.fp_hash);
    }
    */
    if (DB.key_arr != NULL) {
        DB.key_arr[i].size = key.size;
        DB.key_arr[i].type = key.type;
        print_dbg_msg("before malloc\n");
        DB.key_arr[i].data = (unsigned char*)malloc(key.size);
        if (!DB.key_arr[i].data) {
            print_err_msg("(unsigned char*)malloc(key.size) failed\n");return-1;
        }
        memcpy(DB.key_arr[i].data, key.data, key.size);
        // memcpy(&DB.key_arr[i], &key, sizeof(Key)); 
    }
    if (DB.htObj != NULL) {
        hidx_t idx = insert_entry(DB.htObj, key.data, key.size);
        DB.EntryID_arr_htidx[idx] = i;
    }

    if (val.type == STRING) {
        char *val_data = (char*)malloc(val.size);
        if (!val_data) { perror("malloc failed"); return -1; }

        memcpy((char*)val_data, (char*)val.data, val.size);
        size_t len;
        if (find_str_nul(val_data, val.size) >= 0) {
            len = strlen(val_data);
        } else {
            len = val.size - 1;
        }
        memset(val_data + len, 0, val.size - len);
        fwrite((unsigned char*)val_data, 1, (size_t)val.size, (FILE*)DB.fp);

        free(val_data);
    } else {
        fwrite((unsigned char*)val.data, 1, (size_t)val.size, (FILE*)DB.fp);
    }

    uint64_t DataEntrySize = GetDataEntrySize(key.size, val.size);
    uint64_t IndexEntryOffset = GetIndexEntryOffset(DB, i);

    PRINT_DBG_MSG(ESC COLOUR_YELLOW "IndexEntryOffset=%llu\n" ESC COLOUR_YELLOW, (unsigned long long)IndexEntryOffset);

    fseek(DB.fp, IndexEntryOffset, SEEK_SET);
    DB.IndexTable[i].KeyHash = kvdb_hash(key.data, key.size);
    DB.IndexTable[i].EntryID = i;
    DB.IndexTable[i].Flags |= FLAG_VALID;
    DB.IndexTable[i].Size = DataEntrySize;
    DB.IndexTable[i].Offset = DB.OffsetPtr;

    //PrintIndexEntry(stderr, DB, i);
//  fwrite(&DB.IndexTable[i], sizeof(DBIndexEntry), 1, (FILE*)DB.fp);

    DB.OffsetPtr += DataEntrySize;  // legacy?
    DB.Header.ValidEntryCount++;
    DB.db_modified = 1;
    DB.Header.IndexTableOffset += DataEntrySize;
    return i++;
    // printf("afteri++\n");
#undef i
}

int DeleteEntry(DBObject* dbp, uint32_t EntryID) {
    PRINT_DBG_MSG("DeleteEntry(%p, %u);\n", (void*)dbp, EntryID);
    if (EntryID >= DB.Header.EntryCount) {
        print_err_msg("Error: Invalid EntryID\n");
        return -1;
    }
    DB.db_modified = 1;
    // if (EntryID >= DB.Header.EntryCapacity) // BUGS in cpp!!!

    /*
    if (DB.HashTable != NULL) {
        DB.HashTable[EntryID] = 0;
        fseek(DB.fp_hash, EntryID * HASH_SIZE, SEEK_SET);
        fwrite(&DB.HashTable[EntryID], HASH_SIZE, 1, DB.fp_hash);
    }
    */
    if (DB.htObj != NULL) {
        hidx_t idx = delete_entry(DB.htObj, DB.key_arr[EntryID].data, DB.key_arr[EntryID].size);
        DB.EntryID_arr_htidx[idx] = -1;
    }

    if (DB.key_arr[EntryID].data) free(DB.key_arr[EntryID].data);
    DB.key_arr[EntryID].data = NULL;
    DB.key_arr[EntryID].size = 0;
    DB.key_arr[EntryID].type = 0;

    uint64_t IndexEntryOffset = (uint64_t)GetIndexEntryOffset(DB, EntryID);
    DB.IndexTable[EntryID].KeyHash = 0;
    DB.IndexTable[EntryID].Flags = FLAG_DELETED;
    DB.IndexTable[EntryID].Offset = 0;
    DB.IndexTable[EntryID].Size = 0;
    //fseek(DB.fp, IndexEntryOffset, SEEK_SET);
    //fwrite(&DB.IndexTable[EntryID], sizeof(DBIndexEntry), 1, DB.fp);
    //printf(ESC COLOUR_RED "Entry[%u] got deleted\n" ESC RESET_COLOUR, EntryID);
    DB.Header.ValidEntryCount--;
    return EntryID;
}

#define db (*dbp)
void WriteDBEOFHeader(DBObject* dbp) {
    db.Header.EOFHeaderOffset =
        db.Header.IndexTableOffset +
        db.Header.EntryCount * db.Header.IndexEntrySize;
    fseek(db.fp, db.Header.EOFHeaderOffset, SEEK_SET);
    fwrite(&DBEOFMagic, sizeof(DBEOFHeader), 1, DB.fp);
}
void WriteDBHeader(DBObject* dbp) {
    PRINT_DBG_MSG("WriteDBHeader(%p);\n", (void*)dbp);
    //PrintHeader(stdout, db.Header);
    fseek(db.fp, 0, SEEK_SET);
    fwrite(&db.Header, HEADER_SIZE, 1, db.fp);
}

void WriteDBIndexTable(DBObject* dbp) {
    PRINT_DBG_MSG("WriteDBIndexTable(%p);\n", (void*)dbp);
    fseek(DB.fp, DB.Header.IndexTableOffset, SEEK_SET);
    for (ulong_t i = 0; i < DB.Header.EntryCount; i++) {
        fwrite(&DB.IndexTable[i], INDEX_ENTRY_SIZE, 1, DB.fp);
    }
}
#undef db

KVPair *ReadDBEntry(DBObject* dbp, uint32_t EntryID) {
    if (EntryID >= DB.Header.EntryCapacity) {
        print_err_msg(ESC COLOUR_RED "Error: Invalid EntryID\n" ESC RESET_COLOUR);
        return NULL;
    }
    if (DB.IndexTable[EntryID].Flags & FLAG_DELETED) {
        print_err_msg(ESC COLOUR_RED "\n[WARN] Entry[%u] was deleted\n" ESC RESET_COLOUR, EntryID);
        return NULL;
    }

    fseek(DB.fp, DB.IndexTable[EntryID].Offset, SEEK_SET);
    DataEntryHeader RecordHeader = {0};
    size_t fread_cnt = fread(&RecordHeader, sizeof(DataEntryHeader), 1, DB.fp);
    if (fread_cnt != 1) {
        print_err_msg("[ERROR] fread(&RecordHeader, sizeof(DataEntryHeader), 1, DB.fp) != 1\n");
        return NULL;
    }
    //PrintRecordHeader(stdout, RecordHeader, EntryID);
    if (RecordHeader.KeySize > MAX_KEY_SIZE) {
        print_err_msg("[ERROR] Invalid Entry: RecordHeader.KeySize > MAX_KEY_SIZE\n");
        return NULL;
    }


    KVPair *kv = (KVPair*)calloc(1, sizeof(KVPair));
    if (!kv) { print_err_msg("(KVPair*)calloc(1, sizeof(KVPair)) failed\n"); return NULL; }

    void* key = (void*)malloc(RecordHeader.KeySize * sizeof(char));
    void* val = (void*)malloc(RecordHeader.ValSize * sizeof(char));

    if (!key || !val) {
        perror("malloc");
        goto KVPair_ReadDBEntry_v2_failed_cleanup;
    }

    kv->key.size = RecordHeader.KeySize;
    kv->key.type = RecordHeader.KeyType;
    kv->key.data = key;

    kv->val.size = RecordHeader.ValSize;
    kv->val.type = RecordHeader.ValType;
    kv->val.data = val;

    fread_cnt = fread((unsigned char*)key, RecordHeader.KeySize, 1, DB.fp);
    if (fread_cnt != 1) {
        print_dbg_msg("fread((unsigned char*)key, RecordHeader.KeySize, 1, DB.fp) != 1\n");
        goto KVPair_ReadDBEntry_v2_failed_cleanup;
    }
    fread_cnt = fread((unsigned char*)val, RecordHeader.ValSize, 1, DB.fp);
    if (fread_cnt != 1) {
        print_dbg_msg("fread((unsigned char*)val, RecordHeader.ValSize, 1, DB.fp) != 1\n");
        goto KVPair_ReadDBEntry_v2_failed_cleanup;
    }
    // # EndPrintDbgMsg
KVPair_ReadDBEntry_v2_return_success:
    return kv;
KVPair_ReadDBEntry_v2_failed_cleanup:
    if (key) free(key); key = NULL;
    if (val) free(val); val = NULL;
    free(kv);
    return NULL;
#undef MAX_VAL_SIZE
}

void PrintKvPair(KVPair *kv) {
    if (!kv || !kv->key.data || !kv->val.data) return;

    // # PrintDbgMsg
    putchar('\n');
    if (kv->key.type == STRING) {
        printf("key.data=\"%s\"\n", (char*)kv->key.data);
    } else {
        char* buffer = (char*)malloc(kv->key.size * 8ull + 256);
        if (!buffer) { perror("malloc failed"); return; }

        unsigned char* _key = (unsigned char*)kv->key.data;
        strcpy(buffer, "key.data=");
        append_str_hex(&buffer, _key, kv->key.size);
        fputs(buffer, stdout);

        free(buffer);
    }
#define MAX_VAL_SIZE 1024u // This caps the max val size that can be printed
    if (!(kv->val.size < MAX_VAL_SIZE)) {
        printf(ESC COLOUR_YELLOW "# [WARN] val.data cannot be printed: val.size exceeds %u\n" ESC RESET_COLOUR, MAX_VAL_SIZE);
        return;
    }

    if (kv->val.type == STRING) {
        printf("val.data=\"%s\"\n", (char*)kv->val.data);
    } else {
        char* buffer = (char*)malloc(kv->val.size * 8ull + 256);
        if (!buffer) { perror("malloc failed"); return; }

        unsigned char* _val = (unsigned char*)kv->val.data;
        strcpy(buffer, "val.data=\\\n");
        append_str_hex(&buffer, _val, kv->val.size);
        strputc('\n', buffer);
        fputs(buffer, stdout);

        free(buffer);
    }

}

void DestroyKVPair(KVPair *kv) {
    if (!kv) return;
    if (kv->key.data) free(kv->key.data); kv->key.data = NULL;
    if (kv->val.data) free(kv->val.data); kv->val.data = NULL;
    free(kv);
}


KVPair* ReadDBEntryKey(DBObject* dbp, Key key) {
    // This function searches an entry by reading the key
    uint32_t id;
    //int ret;
    //ret = db_key_to_id(&DB, key, &id); // conv key into id (idx of index table)
    id = kvdb_search_key_id(&DB, key);
    if (id < 0) {
        print_err_msg(ESC COLOUR_RED "Error: Invalid key\n" ESC RESET_COLOUR); // Error
        return NULL;
    }
    KVPair *kv = ReadDBEntry(&DB, (unsigned int)id); // read db entry by idx
    return kv;
}
void DeleteDBEntryKey(DBObject* dbp, Key key) {
    // This function searches an entry by reading the key
    uint32_t id;
    //int ret;
    //ret = db_key_to_id(&DB, key, &id); // conv key into id (idx of index table)
    id = kvdb_search_key_id(&DB, key);
    if (id < 0) {
        print_err_msg(ESC COLOUR_RED "Error: Invalid key\n" ESC RESET_COLOUR); // Error
        return;
    }
    DeleteEntry(&DB, (ulong_t)id); // read db entry by idx
}

#undef DB