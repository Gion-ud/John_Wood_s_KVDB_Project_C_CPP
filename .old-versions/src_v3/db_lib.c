#include "../include/db_utils.h"

#define DB (*dbp)
#define STACK_MEM_SIZE_MAX 1024u * 1024u

static const uint8_t DBFileMagic[MAGIC_SIZE] = { 'K', 'V', 'D', 'B', '\r', '\n', ' ', '\0' };

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
static inline void strputc(char c, char* str) {
//  This function appends a single char to a string
    size_t len = strlen(str);
    str[len++] = c;
    str[len] = '\0';
}
static inline uint64_t GetDataEntrySize(uint32_t KeySize, uint64_t ValSize) {
    return (uint64_t) sizeof(DataEntryHeader) + KeySize + ValSize;
//  # key-val entry layout: \
    [DataEntryHeader]       \
    [key.data]              \
    [val.data]
}
static inline uint64_t GetIndexEntryOffset(DBObject db, uint32_t EntryID) {
    return (uint64_t) db.Header.HeaderSize + EntryID * db.Header.IndexEntrySize;
//  EntryID == idx of entries
//  [Header]                        \
    [IndexEntry[0]]                 \
    [IndexEntry[1]]                 \
    [IndexEntry[2]]                 \
	...                             \
    [IndexEntry[idx - 1]]           \
    [IndexEntry[idx]]               \
    [IndexEntry[idx + 1]]           \
    ...                             \
    [IndexEntry[MaxEntryCount - 1]] \
//                                  \
    [DataSection]                   \
    ...
}
static char* conv_bytes_hex(const unsigned char* bytes, size_t size) {
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

    return (char*) buffer;
    // needs to be freed!
}
static inline void append_str_hex(char** dest, unsigned char* src, size_t src_size) {
    char* tmp = (char*)conv_bytes_hex(src, src_size);
    strcat(*dest, tmp);
    free(tmp);
}
static void PrintHeader(FILE* fp, DBFileHeader Header) {
//  This is a helper function that prints the header
    if (fp == stdout) { PRINT_DBG_MSG(ESC COLOUR_CYAN); }
    if (fp == stderr) { PRINT_DBG_MSG(ESC COLOUR_MAGENTA); }

    fputs("# Header\n", fp);
    fputs("Header.Magic=", fp );
    {
        char* tmp = (char*)conv_bytes_hex(Header.Magic, MAGIC_SIZE);
        fputs(tmp, fp);
        free(tmp);
    }
    fprintf(
        fp,
        "Header.Version=%u\n"
        "Header.ByteOrder=%u\n"
        "Header.HeaderSize=%llu\n"
        "Header.IndexEntrySize=%llu\n"
        "Header.EntryCount=%u\n"
        "Header.ValidEntryCount=%u\n"
        "Header.MaxEntryCount=%u\n"
        "Header.DataSectionOffset=%llu\n\n",
        Header.Version,
        Header.ByteOrder,
        (unsigned long long)Header.HeaderSize,
        (unsigned long long)Header.IndexEntrySize,
        Header.EntryCount,
        Header.ValidEntryCount,
        Header.MaxEntryCount,
        (unsigned long long)Header.DataSectionOffset
    );
    if (fp == stderr || fp == stdout) { PRINT_DBG_MSG(ESC RESET_COLOUR); }
}
static inline void PrintIndexEntry(FILE* fp, DBObject db, uint32_t EntryID) {
    if (fp == stderr) { PRINT_DBG_MSG(ESC COLOUR_CYAN); }
    fprintf(fp, "\n# IndexTable[%u]\n", EntryID);
    fprintf(
        fp,
        "IndexTable[%u].ID=%u\n"
        "IndexTable[%u].Flags=0x%08X\n"
        "IndexTable[%u].Size=%llu\n"
        "IndexTable[%u].Offset=%llu\n\n",
        EntryID, db.IndexTable[EntryID].ID,
        EntryID, db.IndexTable[EntryID].Flags,
        EntryID, (unsigned long long)db.IndexTable[EntryID].Size,
        EntryID, (unsigned long long)db.IndexTable[EntryID].Offset
    );
    if (fp == stderr) { PRINT_DBG_MSG(ESC RESET_COLOUR); }
}
static void PrintIndexTable(FILE* fp, DBObject db) {
    fputs("# IndexTable\n", fp);
    for (int i = 0; i < db.Header.EntryCount; i++) {
        fread(&db.IndexTable[i], INDEX_ENTRY_SIZE, 1, db.fp);
    #define EntryID i
        fprintf(
            fp,
            "IndexTable[%u].ID=%u\n"
            "IndexTable[%u].Flags=0x%08X\n"
            "IndexTable[%u].Size=%llu\n"
            "IndexTable[%u].Offset=%llu\n\n",
            EntryID, db.IndexTable[EntryID].ID,
            EntryID, db.IndexTable[EntryID].Flags,
            EntryID, (unsigned long long)db.IndexTable[EntryID].Size,
            EntryID, (unsigned long long)db.IndexTable[EntryID].Offset
        );
    #undef EntryID
    };
}
static void PrintRecordHeader(FILE* fp, DataEntryHeader RecordHeader, uint32_t EntryID) {
    if (fp == stderr) { PRINT_DBG_MSG(ESC COLOUR_MAGENTA); }
    fprintf(fp, "\n# Record[%u]\n", EntryID);
    fprintf(
        fp,
        "RecordHeader.KeySize=%u\n"
        "RecordHeader.KeyType=0x%08X\n"
        "RecordHeader.ValSize=%llu\n"
        "RecordHeader.ValType=0x%08X\n",
        RecordHeader.KeySize,
        RecordHeader.KeyType,
        (unsigned long long)RecordHeader.ValSize,
        RecordHeader.ValType
    );
    if (fp == stderr) { PRINT_DBG_MSG(ESC RESET_COLOUR); }
}
static inline long get_db_size(DBObject db) {
    fseek(db.fp, 0, SEEK_END);
    long db_size = (long)ftell(db.fp);
    fseek(db.fp, db.OffsetPtr, SEEK_SET);
    return db_size;
}
static inline void print_hash_table(DBObject db);

//# no more helper functions below
// Below is an FNV hash function
// #define HASH_TABLE_SIZE db.Header.MaxEntryCount * HASH_SIZE

#define FNV_OFFSET_BASIS    14695981039346656037ULL
#define FNV_PRIME           1099511628211ULL
#define hash_str(key) hash(key, strlen(key) + 1)
hash_t hash(const unsigned char* key, size_t len) {
    hash_t hash_val = FNV_OFFSET_BASIS;
    for (size_t i = 0; i < len; i++) {
        hash_val ^= key[i]; // XOR hash_val with current key byte
        hash_val *= FNV_PRIME; // Multiply hash_val with FNV_PRIME
    }
    return hash_val;
}
#undef HASH_INIT_NUM
#undef HASH_MUL_NUM



long int search_key(DBObject db, Key key) {
#define HASH_TABLE_SIZE db.Header.MaxEntryCount
    hash_t h = hash((unsigned char*)key.data, (uint32_t)key.size);
    size_t idx = h % HASH_TABLE_SIZE;
    for (int i = 0; i < db.Header.MaxEntryCount; i++) {
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


DBObject DBInit(const char* filepath, uint32_t MaxEntryCount) {
//  # This function creates a new db object\
    # DBInit is used to create a new on disk db and DBOpen is used to open an existing db\
    # Init , open and close for Hash table on disk IO are done by separate functions

    DBObject db;
    db.filepath = (char*)malloc(FILE_PATH_SIZE);
    if (!db.filepath) { perror("malloc"); exit(EXIT_FAILURE); }
    memcpy((char*)db.filepath, filepath, FILE_PATH_SIZE); // \
    # This stores file path string to DBObject;\
    # needs to be freed when the obj gets destroyed

    db.fp = fopen(db.filepath, "wb+");
    // This opens on disk db file
    if (!db.fp) { perror("fopen"); exit(EXIT_FAILURE); }

    // Write known metadata to header
    memcpy((uint8_t*)db.Header.Magic, DBFileMagic, MAGIC_SIZE);
    db.Header.Version = VERSION;
    db.Header.ByteOrder = LE;
    db.Header.HeaderSize = HEADER_SIZE;
    db.Header.IndexEntrySize = INDEX_ENTRY_SIZE;
    db.Header.EntryCount = 0;   // Also used as counter/ID for entry count
    db.Header.ValidEntryCount = 0;
    db.Header.MaxEntryCount = MaxEntryCount;
    db.Header.DataSectionOffset = HEADER_SIZE + MaxEntryCount * INDEX_ENTRY_SIZE;

    PrintHeader(stderr, db.Header);

    db.HashTable = (hash_t*)malloc(db.Header.MaxEntryCount * sizeof(hash_t));
    if (!db.HashTable) { PRINT_DBG_MSG("hash_t*)malloc(db.Header.MaxEntryCount * sizeof(hash_t)) failed\n"); exit(EXIT_FAILURE); }
    memset(db.HashTable, 0, db.Header.MaxEntryCount * HASH_SIZE);

    db.IndexTable = (DBIndexEntry*)calloc(MaxEntryCount, INDEX_ENTRY_SIZE); //\
    # This line allocates the IndexTable in db obj;\
    # it will be eventually written to disk
    if (!db.IndexTable) {
        PRINT_DBG_MSG("calloc(MaxEntryCount * INDEX_ENTRY_SIZE) failed\n");
        exit(EXIT_FAILURE);
    }

    db.key_arr = (Key*)calloc(MaxEntryCount, sizeof(Key));
    if (!db.key_arr) {
        PRINT_DBG_MSG("(Key*)calloc(MaxEntryCount, sizeof(Key)) failed\n");
        exit(EXIT_FAILURE);
    }

    db.OffsetPtr = db.Header.DataSectionOffset;
    PRINT_DBG_MSG(ESC COLOUR_YELLOW"\ndb.fp.EOF_Offset=%ld\n" ESC RESET_COLOUR, get_db_size(db));
    return db;
}

DBObject DBOpen(const char* filepath) {
//  This function opens an existing on disk db file and creates db object
    DBObject db = {0};
    db.filepath = (char*)malloc(FILE_PATH_SIZE);
    if (!db.filepath) { perror("malloc(FILE_PATH_SIZE)"); exit(EXIT_FAILURE); }
    memcpy((char*)db.filepath, filepath, FILE_PATH_SIZE);

    db.fp = fopen(db.filepath, "rb+");
    if (!db.fp) { perror("fopen(db.filepath, \"rb+\")"); exit(EXIT_FAILURE); }
    // if (!db.fp) { db.fp = fopen(db.filepath, "wb+"); }
    rewind(db.fp);
    fread(&db.Header, HEADER_SIZE, 1, db.fp);
    if (memcmp(db.Header.Magic, DBFileMagic, MAGIC_SIZE) != 0) {
        PRINT_DBG_MSG(ESC COLOUR_RED "Fatal Error: Invalid DB FILE\n" ESC RESET_COLOUR);
        exit(EXIT_FAILURE);
    }
    PrintHeader(stdout, db.Header);

    db.HashTable = (hash_t*)malloc(db.Header.MaxEntryCount * sizeof(hash_t));
    if (!db.HashTable) { PRINT_DBG_MSG("hash_t*)malloc(db.Header.MaxEntryCount * sizeof(hash_t)) failed\n"); exit(EXIT_FAILURE); }
    memset(db.HashTable, 0, db.Header.MaxEntryCount * HASH_SIZE);

    db.IndexTable = (DBIndexEntry*)calloc(db.Header.MaxEntryCount, INDEX_ENTRY_SIZE);
    if (!db.IndexTable) {
        perror("calloc(MaxEntryCount * INDEX_ENTRY_SIZE)");
        exit(EXIT_FAILURE);
    }
    PrintIndexTable(stdout, db);

    db.key_arr = (Key*)calloc(db.Header.MaxEntryCount, sizeof(Key));
    if (!db.key_arr) {
        PRINT_DBG_MSG("(Key*)calloc(MaxEntryCount, sizeof(Key)) failed\n");
        exit(EXIT_FAILURE);
    }
    fseek(db.fp, db.Header.DataSectionOffset, SEEK_SET);
    DataEntryHeader RecHeader;
    for (uint32_t i = 0; i < db.Header.MaxEntryCount; i++) {
        fread(&RecHeader, sizeof(DataEntryHeader), 1, db.fp);
        db.key_arr[i].data = (char*)malloc(RecHeader.KeySize);
        if (!db.key_arr[i].data) { PRINT_DBG_MSG("(char*)malloc(RecHeader.KeySize) failed\n"); break; }
        fread(db.key_arr[i].data, RecHeader.KeySize, 1, db.fp);
        db.key_arr[i].size = RecHeader.KeySize;
        db.key_arr[i].type = RecHeader.KeyType;
        printf(
            "db.key_arr[%u].data=%.*s\n"
            "db.key_arr[%u].size=%u\n"
            "db.key_arr[%u].type=%u\n\n",
            i, db.key_arr[i].size, (unsigned char*)db.key_arr[i].data,
            i, db.key_arr[i].size,
            i, db.key_arr[i].type
        );
        fseek(db.fp, RecHeader.ValSize, SEEK_CUR);
    }

    db.OffsetPtr = db.Header.DataSectionOffset;
    PRINT_DBG_MSG(ESC COLOUR_YELLOW "\ndb.fp.EOF_Offset=%ld\n" ESC RESET_COLOUR, get_db_size(db));
    return db;
}

int init_file_hash_table(DBObject* dbp, const char* filepath) {
    DB.fp_hash = fopen(filepath, "wb+");
    if (!DB.fp_hash) { perror("fopen"); return -1; }
    if (!DB.HashTable) return -1;
    // DB.HashTable = (hash_t*)malloc(DB.Header.MaxEntryCount * sizeof(hash_t));

    fseek(DB.fp_hash, 0, SEEK_SET);
    return DB.Header.MaxEntryCount;
}
int open_file_hash_table(DBObject* dbp, const char* filepath) {
    DB.fp_hash = fopen(filepath, "rb+");
    if (!DB.fp_hash) { perror("fopen"); return -1; }
    if (!DB.HashTable) return -1;
    // DB.HashTable = (hash_t*)malloc(DB.Header.MaxEntryCount * sizeof(hash_t));

    fseek(DB.fp_hash, 0, SEEK_SET);
    for (uint32_t i = 0; i < DB.Header.MaxEntryCount; i++) {
        fread(&DB.HashTable[i], HASH_SIZE, 1, DB.fp_hash);
        printf("HashTable[%.4u]=0x%.16llx;hash_idx=%.04llu\n", i, DB.HashTable[i], DB.HashTable[i] % DB.Header.MaxEntryCount);
    }

    return DB.Header.MaxEntryCount;
}
void close_file_hash_table(DBObject* dbp) {
    if (DB.fp_hash) fclose(DB.fp_hash);
}

void CloseDB(DBObject* dbp) {
    PRINT_DBG_MSG("CloseDB(%p);\n", (void*)dbp);
    for (uint32_t i = 0; i < DB.Header.MaxEntryCount; i++) {
        if (DB.key_arr[i].data) free(DB.key_arr[i].data);
    }
    free(DB.key_arr);
    free(DB.HashTable);
    free(DB.IndexTable);
    free(DB.filepath);
    fclose(DB.fp);
}

void InsertEntry(DBObject* dbp, Key key, Val val) {
#define i (uint32_t)DB.Header.EntryCount
    PRINT_DBG_MSG(ESC COLOUR_YELLOW "i=%u;max=%u\n" ESC RESET_COLOUR, i, DB.Header.MaxEntryCount);

    if (i >= DB.Header.MaxEntryCount) {
        PRINT_DBG_MSG(ESC COLOUR_RED "Error: DB full\n" ESC RESET_COLOUR);
        return;
    }
    if (key.size > MAX_KEY_SIZE) {
        PRINT_DBG_MSG(ESC COLOUR_RED "# key.size exceeds MAX_KEY_SIZE:%u\n" ESC RESET_COLOUR, MAX_KEY_SIZE);
        return;
    }

    DataEntryHeader RecordHeader = {0};
    RecordHeader.KeySize = key.size;
    RecordHeader.KeyType = key.type;
    RecordHeader.ValSize = val.size;
    RecordHeader.ValType = val.type;

    fseek(DB.fp, DB.OffsetPtr, SEEK_SET);

    // # PrintDbgMsg
    PRINT_DBG_MSG(ESC COLOUR_YELLOW "\nInserting Entry: Entry[%u]\n" ESC RESET_COLOUR, i);
    PRINT_DBG_MSG(ESC COLOUR_YELLOW "CurrentOffset=%llu\n" ESC RESET_COLOUR, (unsigned long long)DB.OffsetPtr);
    // # EndPrintDbgMsg

    fwrite((DataEntryHeader*)&RecordHeader, sizeof(RecordHeader), 1, (FILE*)DB.fp);

    if (key.type == STRING) {
        char *key_data = (char*)malloc(key.size);
        if (!key_data) { perror("malloc failed"); CloseDB(&DB); exit(EXIT_FAILURE); }

        memcpy((char*)key_data, (char*)key.data, key.size);
        size_t len;
        if (find_str_nul(key_data, key.size) >= 0) {
            len = strlen(key_data);
        } else {
            len = key.size - 1;
        }
        memset(key_data + len, 0, key.size - len);
        fwrite((void*)key_data, (size_t)key.size, 1, (FILE*)DB.fp);

        free(key_data);
    } else {
        fwrite((void*)key.data, (size_t)key.size, 1, (FILE*)DB.fp);
    }

    if (DB.HashTable != NULL) {
        DB.HashTable[i] = hash(key.data, key.size);
        printf("DB.HashTable[%.4u]=0x%.16llx;idx=%.4llu\n", i, DB.HashTable[i], DB.HashTable[i] % DB.Header.MaxEntryCount);
        fwrite(&DB.HashTable[i], HASH_SIZE, 1, DB.fp_hash);
    }
    if (DB.key_arr != NULL) {
        DB.key_arr[i].size = key.size;
        DB.key_arr[i].type = key.type;
        DB.key_arr[i].data = (unsigned char*)malloc(key.size);
        memcpy(DB.key_arr[i].data, key.data, key.size);
        // memcpy(&DB.key_arr[i], &key, sizeof(Key)); 
    }

    if (val.type == STRING) {
        char *val_data = (char*)malloc(val.size);
        if (!val_data) { perror("malloc failed"); CloseDB(&DB); exit(EXIT_FAILURE); }

        memcpy((char*)val_data, (char*)val.data, val.size);
        size_t len;
        if (find_str_nul(val_data, val.size) >= 0) {
            len = strlen(val_data);
        } else {
            len = val.size - 1;
        }
        memset(val_data + len, 0, val.size - len);
        fwrite((void*)val_data, (size_t)val.size, 1, (FILE*)DB.fp);

        free(val_data);
    } else {
        fwrite((void*)val.data, (size_t)val.size, 1, (FILE*)DB.fp);
    }



    uint64_t DataEntrySize = GetDataEntrySize(key.size, val.size);
    uint64_t IndexEntryOffset = GetIndexEntryOffset(DB, i);

    PRINT_DBG_MSG(ESC COLOUR_YELLOW "IndexEntryOffset=%llu\n" ESC COLOUR_YELLOW, (unsigned long long)IndexEntryOffset);

    fseek(DB.fp, IndexEntryOffset, SEEK_SET);
    DB.IndexTable[i].ID = i;
    DB.IndexTable[i].Flags |= FLAG_VALID;
    DB.IndexTable[i].Size = DataEntrySize;
    DB.IndexTable[i].Offset = DB.OffsetPtr;

    PrintIndexEntry(stderr, DB, i);
    fwrite(&DB.IndexTable[i], sizeof(DBIndexEntry), 1, (FILE*)DB.fp);

    DB.OffsetPtr += DataEntrySize;
    DB.Header.ValidEntryCount++;
    i++;

    // printf("afteri++\n");
#undef i
}

void DeleteEntry(DBObject* dbp, uint32_t EntryID) {
    PRINT_DBG_MSG("DeleteEntry(%p, %u);\n", (void*)dbp, EntryID);
    if (EntryID >= DB.Header.MaxEntryCount) {
        PRINT_DBG_MSG("Error: Invalid EntryID\n");
        return;
    }
    DB.IndexTable[EntryID].Flags = FLAG_DELETED;
    if (DB.HashTable != NULL) {
        DB.HashTable[EntryID] = 0;
        fseek(DB.fp_hash, EntryID * HASH_SIZE, SEEK_SET);
        fwrite(&DB.HashTable[EntryID], HASH_SIZE, 1, DB.fp_hash);
    }
    uint64_t IndexEntryOffset = (uint64_t)GetIndexEntryOffset(DB, EntryID);
    fseek(DB.fp, IndexEntryOffset, SEEK_SET);
    fwrite(&DB.IndexTable[EntryID], sizeof(DBIndexEntry), 1, DB.fp);
    printf(ESC COLOUR_RED "Entry[%u] got deleted\n" ESC RESET_COLOUR, EntryID);
    DB.Header.ValidEntryCount--;
}

void WriteDBHeader(DBObject* dbp) {
    PRINT_DBG_MSG("WriteDBHeader(%p);\n", (void*)dbp);
    #define db (*dbp)
    PrintHeader(stdout, db.Header);
    fseek(db.fp, 0, SEEK_SET);
    fwrite(&db.Header, HEADER_SIZE, 1, db.fp);
    #undef db
}

void ReadDBEntry(DBObject* dbp, uint32_t EntryID) {
    if (EntryID >= DB.Header.MaxEntryCount) {
        PRINT_DBG_MSG(ESC COLOUR_RED "Error: Invalid EntryID\n" ESC RESET_COLOUR);
        return;
    }
    PRINT_DBG_MSG("ReadDBEntry(%p, %u);\n", dbp, EntryID);
    PrintIndexEntry(stderr, DB, EntryID);

    if (DB.IndexTable[EntryID].Flags & FLAG_DELETED) {
        PRINT_DBG_MSG(ESC COLOUR_RED "\n# Entry[%u] was deleted\n" ESC RESET_COLOUR, EntryID);
        return;
    }

    fseek(DB.fp, DB.IndexTable[EntryID].Offset, SEEK_SET);
    DataEntryHeader RecordHeader = {0};
    fread(&RecordHeader, sizeof(DataEntryHeader), 1, DB.fp);

    PrintRecordHeader(stdout, RecordHeader, EntryID);

    void* key = (void*)malloc(RecordHeader.KeySize * sizeof(char));
    void* val = (void*)malloc(RecordHeader.ValSize * sizeof(char));
    if (!key || !val) { perror("malloc"); return; }

    // # PrintDbgMsg
    putchar('\n');
    if (RecordHeader.KeyType == STRING) {
        fread((char*)key, RecordHeader.KeySize, 1, DB.fp);
        printf("key.data=\"%s\"\n", (char*)key);
    } else {
        char* buffer = (char*)malloc(RecordHeader.ValSize * 8ull + 256);
        if (!buffer) { perror("malloc failed"); return; }

        fread((unsigned char*)key, RecordHeader.KeySize, 1, DB.fp);

        unsigned char* _key = (unsigned char*)key;
        strcpy(buffer, "key.data=");
        append_str_hex(&buffer, _key, RecordHeader.KeySize);
        fputs(buffer, stdout);

        free(buffer);
    }
#define MAX_VAL_SIZE 1024u // This caps the max val size that can be printed
    if (!(RecordHeader.ValSize < MAX_VAL_SIZE)) {
        printf(ESC COLOUR_RED "# Warning: val.size exceeds %u thus cannot be printed\n" ESC RESET_COLOUR, MAX_VAL_SIZE);
        free(key);
        free(val);
        return;
    }
#undef MAX_VAL_SIZE
    if (RecordHeader.ValType == STRING) {
        fread((char*)val, RecordHeader.ValSize, 1, DB.fp);
        printf("val.data=\"%s\"\n", (char*)val);
    } else {
        char* buffer = (char*)malloc(RecordHeader.ValSize * 8ull + 256);
        if (!buffer) { perror("malloc failed"); free(key); return; }

        fread((unsigned char*)val, RecordHeader.ValSize, 1, DB.fp);

        unsigned char* _val = (unsigned char*)val;
        strcpy(buffer, "val.data=\\\n");
        append_str_hex(&buffer, _val, RecordHeader.ValSize);
        strputc('\n', buffer);
        fputs(buffer, stdout);

        free(buffer);
    }
    // # EndPrintDbgMsg
    free(key);
    free(val);
}


// This is Extremely inefficient linear scan so imma add hash table
int db_key_to_id(DBObject* dbp, Key key, uint32_t* id) {
//  This function searches a key in the db and returns idx of the entry 
    PRINT_DBG_MSG(ESC COLOUR_YELLOW "db_key_to_id(%p, *(%p), %p);" ESC RESET_COLOUR "\n", dbp, &key, id);
    PRINT_DBG_MSG(
        "\n"
        "key.size=%u\n"
        "key.type=0x%08X\n",
        key.size,
        key.type
    );
    {
        char* tmp = (char*)conv_bytes_hex(key.data, key.size);
        PRINT_DBG_MSG("key.data=%s\n\n", tmp);
        free(tmp);
    }

    fseek(DB.fp, 0, SEEK_SET);
    for (uint32_t i = 0; i < DB.Header.EntryCount; i++) {
        fseek(DB.fp, DB.IndexTable[i].Offset, SEEK_SET);
        DataEntryHeader RecHeader = {0};
        fread(&RecHeader, sizeof(DataEntryHeader), 1, DB.fp);
        PRINT_DBG_MSG(
            ESC COLOUR_BLUE
            "idx=%u\n"
            "RecHeader.KeySize=%u\n"
            "RecHeader.KeyType=0x%08X\n"
            ESC RESET_COLOUR,
            i, RecHeader.KeySize, RecHeader.KeyType
        );

        unsigned char* key_data = (unsigned char*)malloc(key.size);
        if (!key_data) { perror("malloc"); return -1; }
        key_data[0] = 0;
        fread(key_data, key.size, 1, DB.fp);

        {
            // This block of code prints key.data as hex for debugging
            char* tmp = (char*)conv_bytes_hex(key_data, RecHeader.KeySize);
            PRINT_DBG_MSG("key_data=%s\n\n", tmp);
            free(tmp);
        }

        if (key.size != RecHeader.KeySize) continue; // rejects none sense
        if (key.type != RecHeader.KeyType) continue; // rejects none sense

        int ret;
        if (key.type == STRING && find_str_nul(key_data, key.size) >= 0) {
            ret = strncmp(key_data, (unsigned char*)key.data, strlen((unsigned char*)key.data));
            PRINT_DBG_MSG(ESC COLOUR_YELLOW "# ret=%d\n" ESC RESET_COLOUR, ret);
        } else {
            ret = memcmp(key_data, (unsigned char*)key.data, key.size);
            PRINT_DBG_MSG(ESC COLOUR_YELLOW "# ret=%d\n" ESC RESET_COLOUR, ret);
        }

        free(key_data);
        if (ret == 0) { *id = (uint32_t)i; return 0; }
    }

    PRINT_DBG_MSG(ESC COLOUR_RED "Error: Key Not found\n" ESC RESET_COLOUR);
    return -1; // NOT FOUND
}

void ReadDBEntryKey(DBObject* dbp, Key key) {
    // This function searches an entry by reading the key
    uint32_t id;
    int ret = db_key_to_id(&DB, key, &id); // conv key into id (idx of index table)
    if (ret < 0) {
        PRINT_DBG_MSG(ESC COLOUR_RED "Error: Invalid key\n" ESC RESET_COLOUR); // Error
        return;
    }
    ReadDBEntry(&DB, id); // read db entry by idx
}

#undef DB