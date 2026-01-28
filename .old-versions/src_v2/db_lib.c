#include "../include/db_utils.h"

#define DB (*dbp)

static const uint8_t DBFileMagic[MAGIC_SIZE] = { 'K', 'V', 'D', 'B', '\r', '\n', ' ', '\0' };

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
static char* conv_bytes_hex(unsigned char* bytes, size_t size) {
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
        "Header.HeaderSize=%llu\n"
        "Header.IndexEntrySize=%llu\n"
        "Header.EntryCount=%u\n"
        "Header.ValidEntryCount=%u\n"
        "Header.MaxEntryCount=%u\n"
        "Header.DataSectionOffset=%llu\n\n",
        Header.Version,
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



DBObject DBInit(const char* filepath, uint32_t MaxEntryCount) {
//  This function creates a new db object
    DBObject db;
    db.filepath = (char*)malloc(FILE_PATH_SIZE);
    if (!db.filepath) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    memcpy((char*)db.filepath, filepath, FILE_PATH_SIZE); // \
    # This stores file path string to DBObject;\
    # needs to be freed when the obj gets destroyed

    db.fp = fopen(db.filepath, "wb+");
    // This opens on disk db file
    if (!db.fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Write known metadata to header
    memcpy((uint8_t*)db.Header.Magic, DBFileMagic, MAGIC_SIZE);
    db.Header.Version = VERSION;
    db.Header.HeaderSize = HEADER_SIZE;
    db.Header.IndexEntrySize = INDEX_ENTRY_SIZE;
    db.Header.EntryCount = 0;   // Also used as counter/ID for entry count
    db.Header.ValidEntryCount = 0;
    db.Header.MaxEntryCount = MaxEntryCount;
    db.Header.DataSectionOffset = HEADER_SIZE + MaxEntryCount * INDEX_ENTRY_SIZE;

    PrintHeader(stderr, db.Header);

    db.IndexTable = (DBIndexEntry*)calloc(MaxEntryCount, INDEX_ENTRY_SIZE); //\
    # This line allocates the IndexTable in db obj;\
    # it will be eventually written to disk
    if (!db.IndexTable) {
        perror("calloc(MaxEntryCount * INDEX_ENTRY_SIZE)");
        exit(EXIT_FAILURE);
    }

    db.OffsetPtr = db.Header.DataSectionOffset;

    fseek(db.fp, 0, SEEK_END);
    PRINT_DBG_MSG(ESC COLOUR_YELLOW"\ndb.fp.Offset=%lu\n" ESC RESET_COLOUR, (long)ftell(db.fp));
    fseek(db.fp, db.OffsetPtr, SEEK_SET);
    return db;
}

DBObject DBOpen(const char* filepath) {
//  This function opens an existing on disk db project
    DBObject db;
    db.filepath = (char*)malloc(FILE_PATH_SIZE);
    if (!db.filepath) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    memcpy((char*)db.filepath, filepath, FILE_PATH_SIZE);
    db.fp = fopen(db.filepath, "rb+");
    if (!db.fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fread(&db.Header, HEADER_SIZE, 1, db.fp);
    PrintHeader(stdout, db.Header);

    db.IndexTable = (DBIndexEntry*)calloc(db.Header.MaxEntryCount, INDEX_ENTRY_SIZE);
    if (!db.IndexTable) {
        perror("calloc(MaxEntryCount * INDEX_ENTRY_SIZE)");
        exit(EXIT_FAILURE);
    }

    PrintIndexTable(stdout, db);

    db.OffsetPtr = db.Header.DataSectionOffset;

    fseek(db.fp, 0, SEEK_END);
    PRINT_DBG_MSG("# db.fp.Offset=%lu\n", (long)ftell(db.fp));
    fseek(db.fp, db.OffsetPtr, SEEK_SET);
    return db;
}

void CloseDB(DBObject* dbp);
void InsertEntry(DBObject* dbp, Key key, Val val) {
#define i (uint32_t)DB.Header.EntryCount
    PRINT_DBG_MSG(ESC COLOUR_YELLOW "i=%u;max=%u\n" ESC RESET_COLOUR, i, DB.Header.MaxEntryCount);
    if (i >= DB.Header.MaxEntryCount) {
        PRINT_DBG_MSG(ESC COLOUR_RED "Error: DB full\n" ESC RESET_COLOUR);
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

    PRINT_DBG_MSG("IndexEntryOffset=%llu\n", (unsigned long long)IndexEntryOffset);

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
#undef i
}

void DeleteEntry(DBObject* dbp, uint32_t EntryID) {
    PRINT_DBG_MSG("DeleteEntry(%p, %u);\n", (void*)dbp, EntryID);
    if (EntryID >= DB.Header.MaxEntryCount) {
        PRINT_DBG_MSG("Error: Invalid EntryID\n");
        return;
    }
    DB.IndexTable[EntryID].Flags = FLAG_DELETED;
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

void CloseDB(DBObject* dbp) {
    PRINT_DBG_MSG("CloseDB(%p);\n", (void*)dbp);
    free(DB.IndexTable);
    free(DB.filepath);
    fclose(DB.fp);
}

void ReadDBEntry(DBObject* dbp, uint32_t EntryID) {
    if (EntryID >= DB.Header.MaxEntryCount) {
        PRINT_DBG_MSG(ESC COLOUR_RED "Error: Invalid EntryID\n" ESC RESET_COLOUR);
        return;
    }

    PRINT_DBG_MSG("ReadDBEntry(%p, %u);\n", dbp, EntryID);

    PrintIndexEntry(stderr, DB, EntryID);

    if (DB.IndexTable[EntryID].Flags & FLAG_DELETED) {
        PRINT_DBG_MSG("\n# Entry[%u] was deleted\n", EntryID);
        return;
    }

    fseek(DB.fp, DB.IndexTable[EntryID].Offset, SEEK_SET);
    DataEntryHeader RecordHeader = {0};
    fread(&RecordHeader, sizeof(DataEntryHeader), 1, DB.fp);

    // # PrintDbgMsg
    printf("\n# Record[%u]\n", EntryID);
    printf(
        "RecordHeader.KeySize=%u\n"
        "RecordHeader.KeyType=0x%08X\n"
        "RecordHeader.ValSize=%llu\n"
        "RecordHeader.ValType=0x%08X\n",
        RecordHeader.KeySize,
        RecordHeader.KeyType,
        (unsigned long long)RecordHeader.ValSize,
        RecordHeader.ValType
    );
    // # EndPrintDbgMsg

    void* key = (void*)malloc(RecordHeader.KeySize * sizeof(char));
    void* val = (void*)malloc(RecordHeader.ValSize * sizeof(char));
    if (!key || !val) {
        perror("malloc");
        return;
    }

    // # PrintDbgMsg
    putchar('\n');
    if (RecordHeader.KeyType == STRING) {
        fread((char*)key, RecordHeader.KeySize, 1, DB.fp);
        printf("key.data=\"%s\"\n", (char*)key);
    } else {
        char* buffer;
        if (RecordHeader.ValSize < 1024ull * 1024ull) {
            buffer = (char*)alloca(RecordHeader.ValSize * 8ULL + 256ULL);
        } else {
            buffer = (char*)malloc(RecordHeader.ValSize * 8ULL);
            if (!buffer) {
                perror("malloc failed");
                return;
            }
        }
        fread((unsigned char*)key, RecordHeader.KeySize, 1, DB.fp);
        unsigned char* _key = (unsigned char*)key;
        strcpy(buffer, "key.data=");
        {
            char* tmp = (char*)conv_bytes_hex(_key, RecordHeader.KeySize);
            strcat(buffer, tmp);
            free(tmp);
        }
        fputs(buffer, stdout);
        if (RecordHeader.KeySize < 1024ull * 1024ull) {
        } else {
            free(buffer);
        }
    }
#define MAX_VAL_SIZE 256u
    if (!(RecordHeader.ValSize < MAX_VAL_SIZE)) {
        PRINT_DBG_MSG(ESC COLOUR_RED "# val.size exceeds %u\n" ESC RESET_COLOUR, MAX_VAL_SIZE);
        free(key);
        free(val);
        return;
    }
#undef MAX_VAL_SIZE
    if (RecordHeader.ValType == STRING) {
        fread((char*)val, RecordHeader.ValSize, 1, DB.fp);
        printf("val.data=\"%s\"\n", (char*)val);
    } else {
        char* buffer;
        if (RecordHeader.ValSize < 1024ull * 1024ull) {
            buffer = (char*)alloca(RecordHeader.ValSize * 8ULL + 256ULL);
        } else {
            buffer = (char*)malloc(RecordHeader.ValSize * 8ULL);
            if (!buffer) {
                perror("malloc failed");
            }
        }
        fread((unsigned char*)val, RecordHeader.ValSize, 1, DB.fp);
        unsigned char* _val = (unsigned char*)val;
        strcpy(buffer, "val.data=");
        strcat(buffer, "\\\n");
        {
            char* tmp = (char*)conv_bytes_hex(_val, RecordHeader.ValSize);
            strcat(buffer, tmp);
            free(tmp);
        }
        strputc('\n', buffer);
        fputs(buffer, stdout);

        if (RecordHeader.ValSize < 1024ull * 1024ull) {
        } else {
            free(buffer);
        }
    }
    // # EndPrintDbgMsg
    free(key);
    free(val);
}

int db_key_to_id(DBObject* dbp, Key key, uint32_t* id) {
    PRINT_DBG_MSG(ESC COLOUR_YELLOW "db_key_to_id(%p, *(%p), %p);" ESC RESET_COLOUR "\n", dbp, &key, id);
    fputc('\n', stderr);
    PRINT_DBG_MSG(
        "key.size=%u\n"
        "key.type=0x%08X\n",
        key.size,
        key.type
    );
    {
        char* tmp = (char*)conv_bytes_hex(key.data, key.size);
        PRINT_DBG_MSG("key.data=%s\n", tmp);
        free(tmp);
    }
    fputc('\n', stderr);

    fseek(DB.fp, 0, SEEK_SET);
    PRINT_DBG_MSG(ESC COLOUR_BLUE);
    for (uint32_t i = 0; i < DB.Header.MaxEntryCount; i++) {
        fseek(DB.fp, DB.IndexTable[i].Offset, SEEK_SET);
        DataEntryHeader RecHeader;
        fread(&RecHeader, sizeof(DataEntryHeader), 1, DB.fp);
        RecHeader.KeySize;
        RecHeader.KeyType;
        PRINT_DBG_MSG("idx=%u\n", i);
        PRINT_DBG_MSG("RecHeader.KeySize=%u\n", RecHeader.KeySize);
        PRINT_DBG_MSG("RecHeader.KeyType=0x%08X\n", RecHeader.KeyType);
        unsigned char* key_data = (unsigned char*)alloca(key.size);
        key_data[0] = 0;
        fread(key_data, key.size, 1, DB.fp);
        {
            char* tmp = (char*)conv_bytes_hex(key_data, RecHeader.KeySize);
            PRINT_DBG_MSG("key_data=%s\n", tmp);
            free(tmp);
        }
        fputc('\n', stderr);
        if (key.size != RecHeader.KeySize) return -1;
        if (key.type != RecHeader.KeyType) return -1;
        int ret;
        if (key.type == STRING && find_str_nul(key_data, key.size) >= 0) {
            ret = strncmp(key_data, (unsigned char*)key.data, strlen((unsigned char*)key.data));
            PRINT_DBG_MSG("# ret=%d\n", ret);
        } else {
            ret = memcmp(key_data, (unsigned char*)key.data, key.size);
            PRINT_DBG_MSG("# ret=%d\n", ret);
        }
        if (ret == 0) { *id = (uint32_t)i; return 0; }
    }
    PRINT_DBG_MSG(ESC RESET_COLOUR);
    PRINT_DBG_MSG("Error: Key Not found\n");
    return -1;
}

void ReadDBEntryKey(DBObject* dbp, Key key) {
    uint32_t id;
    int ret = db_key_to_id(&DB, key, &id);
    if (ret < 0) {
        PRINT_DBG_MSG("Invalid key\n");
        return;
    }
    ReadDBEntry(&DB, id);
}

#undef DB