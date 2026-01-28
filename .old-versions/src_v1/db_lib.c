#include "../include/db_utils.h"

#ifdef _WIN32
    #define fseek _fseeki64
    #define ftell _ftelli64
#endif

#define DB (*dbp)

static const uint8_t DBFileMagic[MAGIC_SIZE] = { 'K', 'V', 'D', 'B', '\r', '\n', ' ', '\0' };

static inline int find_str_nul(char* buffer, size_t buf_size) {
    for (size_t i = 0; i < buf_size; i++) {
        if (buffer[i] == '\0') {
            return i;
        }
    }
    return -1;
}

static inline void strputc(char c, char* str) {
    size_t len = strlen(str);
    str[len++] = c;
    str[len] = '\0';
}

static inline uint64_t GetDataEntrySize(uint32_t KeySize, uint64_t ValSize) {
    return (uint64_t) sizeof(DataEntryHeader) + KeySize + ValSize;
}

static inline uint64_t GetIndexEntryOffset(DBObject db, uint32_t EntryID) {
    return (uint64_t) db.Header.HeaderSize + EntryID * db.Header.IndexEntrySize;
}

static char* conv_bytes_hex(unsigned char* bytes, size_t size) {
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
            len = strlen(ByteHex);
            ByteHex[len++] = ',';
            ByteHex[len] = '\0';
        } else {
            if (i < size - 1) {
                len = strlen(ByteHex);
                ByteHex[len++] = ',';
                ByteHex[len] = '\0';

                len = strlen(ByteHex);
                ByteHex[len++] = '\\';
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

DBObject DBInit(const char* filepath, uint32_t MaxEntryCount) {
    DBObject db;
    db.filepath = (char*)malloc(FILE_PATH_SIZE);
    if (!db.filepath) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    memcpy((char*)db.filepath, filepath, FILE_PATH_SIZE);
    /*
    db.membuf = (unsigned char*)malloc(MEM_FILE_SIZE);
    if (!db.membuf) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }*/
    // FILE *fmemopen(size_t size; void buf[size], size_t size, const char *mode);
    // db.fp_mem = fmemopen(db.membuf, MEM_FILE_SIZE, "rb+");
    /*
    db.fp_mem = tmpfile();
    if (!db.fp_mem) {
        perror("tmpfile");
        // perror("fmemopen");
        exit(EXIT_FAILURE);
    }
    */
    db.fp = fopen(db.filepath, "wb+");

    if (!db.fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    memcpy((uint8_t*)db.Header.Magic, DBFileMagic, MAGIC_SIZE);
    db.Header.Version = VERSION;
    db.Header.HeaderSize = HEADER_SIZE;
    db.Header.IndexEntrySize = INDEX_ENTRY_SIZE;
    db.Header.EntryCount = 0;   // Also used as counter/ID for entry count
    db.Header.MaxEntryCount = MaxEntryCount;
    db.Header.DataSectionOffset = HEADER_SIZE + MaxEntryCount * INDEX_ENTRY_SIZE;

    //# DebugMsg
    fputs(
        "# Header\n"
        "Header.Magic=",
        stdout
    );
    char* magic_dmp_txt = (char*)conv_bytes_hex(db.Header.Magic, MAGIC_SIZE);
    fputs(magic_dmp_txt, stdout);
    free(magic_dmp_txt);

    printf(
        "Header.Version=%u\n"
        "Header.HeaderSize=%llu\n"
        "Header.IndexEntrySize=%llu\n"
        "Header.EntryCount=%u\n"
        "Header.MaxEntryCount=%u\n"
        "Header.DataSectionOffset=%llu\n",
        db.Header.Version,
        (unsigned long long)db.Header.HeaderSize,
        (unsigned long long)db.Header.IndexEntrySize,
        db.Header.EntryCount,
        db.Header.MaxEntryCount,
        (unsigned long long)db.Header.DataSectionOffset
    );
    //# EndDebugMsg

    db.IndexTable = (DBIndexEntry*)calloc(MaxEntryCount, INDEX_ENTRY_SIZE);
    if (!db.IndexTable) {
        perror("calloc(MaxEntryCount * INDEX_ENTRY_SIZE)");
        exit(EXIT_FAILURE);
    }

    db.OffsetPtr = db.Header.DataSectionOffset;

    fseek(db.fp, 0, SEEK_END);
    fprintf(stderr, "\ndb.fp.Offset=%lu\n", (long)ftell(db.fp));
    fseek(db.fp, db.OffsetPtr, SEEK_SET);
    return db;
}

DBObject DBOpen(const char* filepath) {
    DBObject db;
    db.filepath = (char*)malloc(FILE_PATH_SIZE);
    if (!db.filepath) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    /*
    db.fp_mem = tmpfile();
    if (!db.fp_mem) {
        perror("tmpfile");
        // perror("fmemopen");
        exit(EXIT_FAILURE);
    }*/
    memcpy((char*)db.filepath, filepath, FILE_PATH_SIZE);
    /*
    db.membuf = (unsigned char*)malloc(MEM_FILE_SIZE);
    if (!db.membuf) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }*/
    // FILE *fmemopen(size_t size; void buf[size], size_t size, const char *mode);
    // db.fp_mem = fmemopen(db.membuf, MEM_FILE_SIZE, "rb+");

    db.fp = fopen(db.filepath, "rb+");
    if (!db.fp) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    fread(&db.Header, HEADER_SIZE, 1, db.fp);

    fputs(
        "# Header\n"
        "Header.Magic=",
        stdout
    );
    {
        char* tmp = (char*)conv_bytes_hex(db.Header.Magic, MAGIC_SIZE);
        fputs(tmp, stdout);
        free(tmp);
    }
    putchar('\n');
    printf(
        "Header.Version=%u\n"
        "Header.HeaderSize=%llu\n"
        "Header.IndexEntrySize=%llu\n"
        "Header.EntryCount=%u\n"
        "Header.MaxEntryCount=%u\n"
        "Header.DataSectionOffset=%llu\n\n",
        db.Header.Version,
        (unsigned long long)db.Header.HeaderSize,
        (unsigned long long)db.Header.IndexEntrySize,
        db.Header.EntryCount,
        db.Header.MaxEntryCount,
        (unsigned long long)db.Header.DataSectionOffset
    );

    db.IndexTable = (DBIndexEntry*)calloc(db.Header.MaxEntryCount, INDEX_ENTRY_SIZE);
    if (!db.IndexTable) {
        perror("calloc(MaxEntryCount * INDEX_ENTRY_SIZE)");
        exit(EXIT_FAILURE);
    }

    fputs("# IndexTable\n", stdout);
    for (int i = 0; i < db.Header.EntryCount; i++) {
        fread(&db.IndexTable[i], INDEX_ENTRY_SIZE, 1, db.fp);
        #define EntryID i
        printf(
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

    db.OffsetPtr = db.Header.DataSectionOffset;

    fseek(db.fp, 0, SEEK_END);
    fprintf(stderr, "# db.fp.Offset=%lu\n", (long)ftell(db.fp));
    fseek(db.fp, db.OffsetPtr, SEEK_SET);
    return db;
}

void CloseDB(DBObject* dbp);
void InsertEntry(DBObject* dbp, Key key, Val val) {
    #define i (uint32_t)DB.Header.EntryCount
    // uint32_t i = DB.Header.EntryCount;

    fprintf(stderr, "i=%u;max=%u\n", i, DB.Header.MaxEntryCount);
    if (i >= DB.Header.MaxEntryCount) {
        fprintf(stderr, "Error: DB full\n");
        return;
    }

    DataEntryHeader RecordHeader = {0};
    RecordHeader.KeySize = key.size;
    RecordHeader.KeyType = key.type;
    RecordHeader.ValSize = val.size;
    RecordHeader.ValType = val.type;

    fseek(DB.fp, DB.OffsetPtr, SEEK_SET);

    // # PrintDbgMsg
    fprintf(stderr, "\nInserting Entry: Entry[%u]\n", i);
    fprintf(stderr, "CurrentOffset=%llu\n", (unsigned long long)DB.OffsetPtr);
    // # EndPrintDbgMsg

    fwrite((DataEntryHeader*)&RecordHeader, sizeof(RecordHeader), 1, (FILE*)DB.fp);
    //fputs(key.data, stdout);
    //putchar('\n');
    fflush(stdout);
    if (key.type == STRING) {
        char *key_data = (char*)malloc(key.size);
        if (!key_data) {
            perror("malloc failed");
            CloseDB(&DB);
            exit(EXIT_FAILURE);
        }
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
        if (!val_data) {
            perror("malloc failed");
            CloseDB(&DB);
            exit(EXIT_FAILURE);
        }
        memcpy((char*)val_data, (char*)val.data, val.size);
        size_t len;
        if (find_str_nul(val_data, val.size) >= 0) {
            len = strlen(val_data);
        } else {
            len = val.size - 1;
        }
        memset(val_data + len, 0, val.size - len);
        // val_data[val.size - 1] = '\0';
        fwrite((void*)val_data, (size_t)val.size, 1, (FILE*)DB.fp);
        free(val_data);
    } else {
        fwrite((void*)val.data, (size_t)val.size, 1, (FILE*)DB.fp);
    }


    uint64_t DataEntrySize = GetDataEntrySize(key.size, val.size);

    uint64_t IndexEntryOffset = GetIndexEntryOffset(DB, i);

    // # PrintDbgMsg
    fprintf(stderr, "IndexEntryOffset=%llu\n", (unsigned long long)IndexEntryOffset);
    // # EndPrintDbgMsg

    fseek(DB.fp, IndexEntryOffset, SEEK_SET);
    DB.IndexTable[i].ID = i;
    DB.IndexTable[i].Flags |= FLAG_VALID;
    DB.IndexTable[i].Size = DataEntrySize;
    DB.IndexTable[i].Offset = DB.OffsetPtr;
    // # PrintDbgMsg
    fprintf(
        stderr,
        "\n"
        "IndexTable[%u].ID=%u\n"
        "IndexTable[%u].Flags=0x%08X\n"
        "IndexTable[%u].Size=%llu\n"
        "IndexTable[%u].Offset=%llu\n",
        i, DB.IndexTable[i].ID,
        i, DB.IndexTable[i].Flags,
        i, (unsigned long long)DB.IndexTable[i].Size,
        i, (unsigned long long)DB.IndexTable[i].Offset
    );
    // # EndPrintDbgMsg
    fwrite(&DB.IndexTable[i], sizeof(DBIndexEntry), 1, (FILE*)DB.fp);

    DB.OffsetPtr += DataEntrySize;
    i++;

    #undef i
    //DB.Header.EntryCount = i;
}

void DeleteEntry(DBObject* dbp, uint32_t EntryID) {
    fprintf(stderr, "DeleteEntry(%p, %u);\n", (void*)dbp, EntryID);
    if (EntryID >= DB.Header.MaxEntryCount) {
        fputs("Error: Invalid EntryID\n", stderr);
        return;
    }
    DB.IndexTable[EntryID].Flags = FLAG_DELETED;
    uint64_t IndexEntryOffset = (uint64_t)GetIndexEntryOffset(DB, EntryID);
    fseek(DB.fp, IndexEntryOffset, SEEK_SET);
    fwrite(&DB.IndexTable[EntryID], sizeof(DBIndexEntry), 1, DB.fp);
    printf("Entry[%u] got deleted\n", EntryID);
}

void WriteDBHeader(DBObject* dbp) {
    fprintf(stderr, "WriteDBHeader(%p);\n", (void*)dbp);
    #define db (*dbp)
    fputs("# Header\n", stdout);
    fputs("Header.Magic=", stdout);
    char* magic_dmp_txt = (char*)conv_bytes_hex(db.Header.Magic, MAGIC_SIZE);
    fputs(magic_dmp_txt, stdout);
    free(magic_dmp_txt);
    printf(
        "Header.Version=%u\n"
        "Header.HeaderSize=%llu\n"
        "Header.IndexEntrySize=%llu\n"
        "Header.EntryCount=%u\n"
        "Header.MaxEntryCount=%u\n"
        "Header.DataSectionOffset=%llu\n",
        db.Header.Version,
        (unsigned long long)db.Header.HeaderSize,
        (unsigned long long)db.Header.IndexEntrySize,
        db.Header.EntryCount,
        db.Header.MaxEntryCount,
        (unsigned long long)db.Header.DataSectionOffset
    );
    fseek(db.fp, 0, SEEK_SET);
    fwrite(&db.Header, HEADER_SIZE, 1, db.fp);
    #undef db
}

/*
void CommitDB(DBObject* dbp) {
    fprintf(stderr, "CommitDB(%p);\n", (void*)dbp);
    int c;
    fseek(DB.fp, 0, SEEK_SET);
    fseek(DB.fp_mem, 0, SEEK_SET);
    {
        unsigned long long i = 0;
    loop_start:
        c = fgetc(DB.fp_mem);
        if (c != EOF && i < DB.OffsetPtr) {
            fputc(c, DB.fp);
            i++;
            goto loop_start;
        } else {
            fprintf(stderr, "# CommitDB: i=%llu\n", (unsigned long long)i);
            goto loop_end;
        }
    loop_end:
        fflush(DB.fp);
        fprintf(stderr, "# CommitDB: DB.OffsetPtr=%llu\n", (unsigned long long)DB.OffsetPtr);
        return;
    }
}
*/

void CloseDB(DBObject* dbp) {
    fprintf(stderr, "CloseDB(%p);\n", (void*)dbp);
    free(DB.IndexTable);
    free(DB.filepath);
    fclose(DB.fp);
}


void ReadDBEntry(DBObject* dbp, uint32_t EntryID) {
    if (EntryID >= DB.Header.MaxEntryCount) {
        fputs("Error: Invalid EntryID\n", stderr);
        return;
    }
    // uint64_t IndexEntryOffset = GetIndexEntryOffset(DB, EntryID);
    // fseek(&DB, IndexEntryOffset, SEEK_SET);

    // # PrintDbgMsg
    fprintf(stderr, "ReadDBEntry(%p, %u);\n", dbp, EntryID);
    fflush(stderr);

    printf("\n# IndexTable[%u]\n", EntryID);
    printf(
        "IndexTable[%u].ID=%u\n"
        "IndexTable[%u].Flags=0x%08X\n"
        "IndexTable[%u].Size=%llu\n"
        "IndexTable[%u].Offset=%llu\n",
        EntryID, DB.IndexTable[EntryID].ID,
        EntryID, DB.IndexTable[EntryID].Flags,
        EntryID, (unsigned long long)DB.IndexTable[EntryID].Size,
        EntryID, (unsigned long long)DB.IndexTable[EntryID].Offset
    );

    if (DB.IndexTable[EntryID].Flags & FLAG_DELETED) {
        printf("\n# Entry[%u] was deleted\n", EntryID);
        return;
    }
    // # EndPrintDbgMsg

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

    void* key = (void*)alloca(RecordHeader.KeySize * sizeof(char));
    void* val = (void*)alloca(RecordHeader.ValSize * sizeof(char));

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
            }
        }
        fread((unsigned char*)key, RecordHeader.KeySize, 1, DB.fp);
        unsigned char* _key = (unsigned char*)key;
        strcpy(buffer, "key.data=");
        // strcat(buffer, "\\\n");
        {
            char* tmp = (char*)conv_bytes_hex(_key, RecordHeader.KeySize);
            strcat(buffer, tmp);
            free(tmp);
        }
        fputs(buffer, stdout);
        if (RecordHeader.ValSize < 1024ull * 1024ull) {
        } else {
            free(buffer);
        }
    }
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

}

int db_key_to_id(DBObject* dbp, Key key, uint32_t* id) {
    fputc('\n', stderr);
    fprintf(
        stderr,
        "key.size=%u\n"
        "key.type=0x%08X\n",
        key.size,
        key.type
    );
    {
        char* tmp = (char*)conv_bytes_hex(key.data, key.size);
        fprintf(stderr, "key.data=%s\n", tmp);
        free(tmp);
    }
    putchar('\n');

    fseek(DB.fp, 0, SEEK_SET);
    for (uint32_t i = 0; i < DB.Header.MaxEntryCount; i++) {
        fseek(DB.fp, DB.IndexTable[i].Offset, SEEK_SET);
        DataEntryHeader RecHeader;
        fread(&RecHeader, sizeof(DataEntryHeader), 1, DB.fp);
        RecHeader.KeySize;
        RecHeader.KeyType;
        fprintf(stderr, "RecHeader.KeySize=%u\n", RecHeader.KeySize);
        fprintf(stderr, "RecHeader.KeyType=0x%08X\n", RecHeader.KeyType);
        unsigned char* key_data = (unsigned char*)alloca(key.size);
        key_data[0] = 0;
        fread(key_data, key.size, 1, DB.fp);
        {
            char* tmp = (char*)conv_bytes_hex(key_data, RecHeader.KeySize);
            fprintf(stderr, "key_data=%s\n", tmp);
            free(tmp);
        }
        fputc('\n', stderr);
        if (key.size == RecHeader.KeySize && key.type == RecHeader.KeyType) {
            int ret;
            if (key.type == STRING && find_str_nul(key_data, key.size) >= 0) {
                ret = strncmp(key_data, (unsigned char*)key.data, strlen((unsigned char*)key.data));
                fprintf(stderr, "# ret=%d\n", ret);
            } else {
                ret = memcmp(key_data, (unsigned char*)key.data, key.size);
                fprintf(stderr, "# ret=%d\n", ret);
            }
            if (ret == 0) {
                *id = (uint32_t)i;
                return 0;
            }
        }

    }
    fprintf(stderr, "Error: Key Not found\n");
    return -1;
}

void ReadDBEntryID(DBObject* dbp, uint32_t EntryID) {
    if (EntryID >= DB.Header.MaxEntryCount) {
        fputs("Error: Invalid EntryID\n", stderr);
        return;
    }
    // uint64_t IndexEntryOffset = GetIndexEntryOffset(DB, EntryID);
    // fseek(&DB, IndexEntryOffset, SEEK_SET);

    // # PrintDbgMsg
    fprintf(stderr, "ReadDBEntryID(%p, %u);\n", dbp, EntryID);
    fflush(stderr);


    if (DB.IndexTable[EntryID].Flags & FLAG_DELETED) {
        printf("\n# Entry[%u] was deleted\n", EntryID);
        return;
    }
    // # EndPrintDbgMsg

    fseek(DB.fp, DB.IndexTable[EntryID].Offset, SEEK_SET);
    DataEntryHeader RecordHeader = {0};
    fread(&RecordHeader, sizeof(DataEntryHeader), 1, DB.fp);

    void* key = (void*)alloca(RecordHeader.KeySize * sizeof(char));
    void* val = (void*)alloca(RecordHeader.ValSize * sizeof(char));

    // # PrintDbgMsg
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
        if (RecordHeader.ValSize < 1024ull * 1024ull) {
        } else {
            free(buffer);
        }
    }
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

}
void ReadDBEntryKey(DBObject* dbp, Key key) {
    uint32_t id;
    int ret = db_key_to_id(&DB, key, &id);
    if (ret < 0) {
        fprintf(stderr, "Invalid key\n");
        return;
    }
    ReadDBEntryID(&DB, id);
}

#undef DB