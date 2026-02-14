#include "kvdb.h"
#include "kvdb_defs.h"

#define DB (*dbp)
#define STACK_MEM_SIZE_MAX 1024u * 1024u

static const ubyte_t DBFileMagic[MAGIC_SIZE] = { 'K', 'V', 'D', 'B', '\r', '\n', ' ', '\0' };
static const ubyte_t DBEOFMagic[MAGIC_SIZE] = { '\n', '.', 'D', 'B', 'E', 'O', 'F', '\0' };


static inline void append_str_hex(char** dest, unsigned char* src, size_t src_size) {
    char* tmp = (char*)conv_bytes_hex(src, src_size);
    strcat(*dest, tmp);
    free(tmp);
}

int KVDB_conv_key_entry_id(DBObject* dbp, Key key) {
#define db (*dbp)
    hash_t key_hash = fnv_1a_hash(key.data, key.len);
    int idx = key_hash % db.htObj->ht_cap;
#define bucket db.htObj->ht[idx].bucket
    for (size_t i = 0; i < db.htObj->ht[idx].bucket_size; ++i) {
        if (
            memcmp(
                db.key_arr[bucket[i].entry_id].data, key.data, key.len
            ) == 0 &&
            db.key_arr[bucket[i].entry_id].len == key.len &&
            // type mismatch
            db.key_arr[bucket[i].entry_id].type == key.type
        ) {
            return bucket[i].entry_id;
        }
    }
    return -1;
#undef bucket
#undef db 
}

// # no more helper functions below


//void KVDB_DBObject_close(DBObject *dbp);
static inline void KVDB_DBObject_create_FillDBFileHeader(DBObject *dbp, int EntryCapacity) {
#define db (*dbp)
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
    db.Header.LastModified = 0; // PlaceHolder
#undef db
}

DBObject* KVDB_DBObject_create(const char* filepath, int EntryCapacity) {
    DBObject* dbp = (DBObject*)calloc(1, sizeof(DBObject));
    if (!dbp) {
        print_err_msg("calloc(1, sizeof(DBObject)) failed\n");
        return NULL;
    }
#define db (*dbp)
    db.fp = fopen(filepath, "wb+");
    if (!db.fp) { perror("fopen failed\n"); return NULL; }

    KVDB_DBObject_create_FillDBFileHeader(&db, EntryCapacity);

    db.IndexTable = (DBIndexEntry*)calloc(EntryCapacity, INDEX_ENTRY_SIZE);
    if (!db.IndexTable) {
        print_err_msg("(DBIndexEntry*)calloc(EntryCapacity * INDEX_ENTRY_SIZE) failed\n");
        goto KVDB_DBObject_create_failed_cleanup;
    }

    db.key_arr = (Key*)calloc(EntryCapacity, sizeof(Key));
    if (!db.key_arr) {
        print_err_msg("(Key*)calloc(EntryCapacity, sizeof(Key)) failed\n");
        goto KVDB_DBObject_create_failed_cleanup;
    }

    db.htObj = HASH_INDEX_LIB_HTObject_create(EntryCapacity);
    if (!db.htObj) goto KVDB_DBObject_create_failed_cleanup;

    db.OffsetPtr = db.Header.DataSectionOffset;
    return dbp;
KVDB_DBObject_create_failed_cleanup:
    if (dbp) KVDB_DBObject_close(&db);
    return NULL;
#undef db
}

static int KVDB_DBObject_open_load_keys(DBObject *dbp) {
    if (!dbp) return -1;
#define db (*dbp)
    db.key_arr = (Key*)calloc(db.Header.EntryCapacity, sizeof(Key));
    if (!db.key_arr) {
        print_err_msg("calloc(EntryCapacity, sizeof(Key)) failed\n");
        return -1;
    }

    fseek(db.fp, db.Header.DataSectionOffset, SEEK_SET);
    DataEntryHeader RecHeader = {0};
    for (uint32_t i = 0; i < db.Header.EntryCount; i++) {
        size_t fread_cnt = fread(&RecHeader, sizeof(DataEntryHeader), 1, db.fp);
        if (fread_cnt != 1) {
            print_err_msg("fread(&RecHeader, sizeof(DataEntryHeader), 1, db.fp) != 1\n");
            goto KVDB_DBObject_open_load_keys_failed_cleanup;
        }

        db.key_arr[i].len = RecHeader.KeySize;
        db.key_arr[i].type = RecHeader.KeyType;
        db.key_arr[i].data = (char*)malloc(RecHeader.KeySize);
        if (!db.key_arr[i].data) { print_err_msg("(char*)malloc(RecHeader.KeySize) failed\n"); return -1; }

        fread_cnt = fread(db.key_arr[i].data, RecHeader.KeySize, 1, db.fp);
        if (fread_cnt != 1) {
            print_err_msg("fread(db.key_arr[i].data, RecHeader.KeySize, 1, db.fp) != 1\n");
            goto KVDB_DBObject_open_load_keys_failed_cleanup;
        }
        /*
        print_dbg_msg(\
            "db.key_arr[%u].data='%.*s'\n"\
            "db.key_arr[%u].size=%u\n"\
            "db.key_arr[%u].type=%u\n\n", \
            i, db.key_arr[i].len, (unsigned char*)db.key_arr[i].data, \
            i, db.key_arr[i].len, \
            i, db.key_arr[i].type\
        );
        */


        fseek(db.fp, RecHeader.ValSize, SEEK_CUR);
    }
    return db.Header.EntryCount;
KVDB_DBObject_open_load_keys_failed_cleanup:
    if (!db.key_arr) return -1;
    for (ulong_t i = 0; i < db.Header.EntryCapacity; ++i) {
        if (db.key_arr[i].data) free(db.key_arr[i].data);
    }
    free(db.key_arr); db.key_arr = NULL; return -1;
#undef db
}

DBObject* KVDB_DBObject_open(const char* filepath) {
    DBObject* dbp = (DBObject*)malloc(sizeof(DBObject));
    if (!dbp) { print_err_msg("malloc(sizeof(DBObject)) failed\n"); goto KVDB_DBObject_open_failed_cleanup; }
    memset(dbp, 0, sizeof(DBObject));
#define db (*dbp)
    db.fp = fopen(filepath, "rb+");
    if (!db.fp) {
        //print_err_msg("fopen failed with rb+\n");
        KVDB_DBObject_close(&db);
        dbp = KVDB_DBObject_create(filepath, 4096);
        return dbp;
    }

    size_t fread_cnt = fread(&db.Header, HEADER_SIZE, 1, db.fp);
    if (fread_cnt != 1) {
        print_err_msg("fread(&db.Header, HEADER_SIZE, 1, db.fp) != 1\n");
        goto KVDB_DBObject_open_failed_cleanup;
    }

    if (memcmp(db.Header.Magic, DBFileMagic, MAGIC_SIZE) != 0) {
        print_err_msg(ESC COLOUR_RED "Fatal Error: Invalid DB FILE\n" ESC RESET_COLOUR);
        goto KVDB_DBObject_open_failed_cleanup;
    }

    db.IndexTable = (DBIndexEntry*)calloc(db.Header.EntryCapacity, INDEX_ENTRY_SIZE);
    if (!db.IndexTable) { perror("(DBIndexEntry*)calloc(EntryCapacity * INDEX_ENTRY_SIZE)"); goto KVDB_DBObject_open_failed_cleanup; }
    fseek(db.fp, db.Header.IndexTableOffset, SEEK_SET);
    for (ulong_t i = 0; i < db.Header.EntryCount; i++) {
        fread_cnt = fread(&db.IndexTable[i], INDEX_ENTRY_SIZE, 1, db.fp);
        if (fread_cnt != 1) {
            print_err_msg("fread(&db.IndexTable[i], INDEX_ENTRY_SIZE, 1, db.fp) != 1\n");
            goto KVDB_DBObject_open_failed_cleanup;
        }
    }

    int ret = KVDB_DBObject_open_load_keys(&db);
    if (ret < 0) {
        print_err_msg("KVDB_DBObject_open_load_keys failed\n");
        goto KVDB_DBObject_open_failed_cleanup;
    }

    db.htObj = HASH_INDEX_LIB_HTObject_create(db.Header.EntryCapacity);
    if (!db.htObj) {
        print_err_msg("HASH_INDEX_LIB_HTObject_create failed\n");
        goto KVDB_DBObject_open_failed_cleanup;
    }
    for (ulong_t i = 0; i < db.Header.EntryCount; ++i) {
        ret = HASH_INDEX_LIB_HTObject_insert(db.htObj, db.IndexTable[i].KeyHash, i);
        if (ret < 0) {
            print_err_msg("HASH_INDEX_LIB_HTObject_insert failed\n");
            HASH_INDEX_LIB_HTObject_destroy(db.htObj);
            goto KVDB_DBObject_open_failed_cleanup;
        }
    }

    db.OffsetPtr = db.Header.DataSectionOffset;

    return dbp;
KVDB_DBObject_open_failed_cleanup:
    if (dbp) KVDB_DBObject_close(&db);
    return NULL;
#undef db
}

static void KVDB_DBObject_close_WriteDBFileHeader(DBObject* dbp);
static void KVDB_DBObject_close_WriteDBIndexTable(DBObject* dbp);
static void KVDB_DBObject_close_WriteDBEOFHeader(DBObject* dbp);
void KVDB_DBObject_close(DBObject* dbp) {
    if (!dbp) return;
    if (DB.db_modified) {
        KVDB_DBObject_close_WriteDBIndexTable(&DB);
        KVDB_DBObject_close_WriteDBEOFHeader(&DB);
        KVDB_DBObject_close_WriteDBFileHeader(&DB);
        KVDB_DBObject_PrintFileHeader(stdout, dbp);
    }
    if (DB.key_arr) {
        for (uint32_t i = 0; i < DB.Header.EntryCapacity; i++) {
            if (DB.key_arr[i].data) free(DB.key_arr[i].data);
        }
        free(DB.key_arr);
    }
    if (DB.htObj) HASH_INDEX_LIB_HTObject_destroy(DB.htObj);
    if (DB.IndexTable) free(DB.IndexTable);
    if (DB.fp) fclose(DB.fp);
    free(dbp);
    dbp = NULL;
}

#define db (*dbp)
static void KVDB_DBObject_close_WriteDBFileHeader(DBObject* dbp) {
    fseek(db.fp, 0, SEEK_SET);
    db.Header.LastModified = (qword_t)time(NULL);
    fwrite(&db.Header, HEADER_SIZE, 1, db.fp);
}
static void KVDB_DBObject_close_WriteDBEOFHeader(DBObject* dbp) {
    db.Header.EOFHeaderOffset =
        db.Header.IndexTableOffset +
        db.Header.EntryCount * db.Header.IndexEntrySize;
    fseek(db.fp, db.Header.EOFHeaderOffset, SEEK_SET);
    fwrite(&DBEOFMagic, sizeof(DBEOFHeader), 1, DB.fp);
}
static void KVDB_DBObject_close_WriteDBIndexTable(DBObject* dbp) {
    fseek(DB.fp, DB.Header.IndexTableOffset, SEEK_SET);
    for (ulong_t i = 0; i < DB.Header.EntryCount; i++) {
        fwrite(&DB.IndexTable[i], INDEX_ENTRY_SIZE, 1, DB.fp);
    }
}
#undef db

ulong_t KVDB_DBObject_EntryCount(DBObject* dbp) {
    return dbp->Header.EntryCount;
}

int KVDB_DBObject_put(DBObject* dbp, Key key, Val val) {
#define i (uint32_t)DB.Header.EntryCount
    if (i >= DB.Header.EntryCapacity) {
        print_err_msg(ESC COLOUR_RED "Error: DB full\n" ESC RESET_COLOUR);
        return -1;
    }
    if (key.len > MAX_KEY_SIZE) {
        print_err_msg(ESC COLOUR_RED "Error: key.len exceeds MAX_KEY_SIZE:%u\n" ESC RESET_COLOUR, MAX_KEY_SIZE);
        return -1;
    }

    /*

    hash_t key_hash = fnv_1a_hash(key.data, key.len);
    int h_idx = HASH_INDEX_LIB_HTObject_insert(DB.htObj, key_hash, i);
    if (h_idx < 0) {
        print_err_msg("HASH_INDEX_LIB_HTObject_insert failed");
        return -1;
    };
    */

    hash_t key_hash = fnv_1a_hash(key.data, key.len);
    int ret = KVDB_conv_key_entry_id(&DB, key);
    if (ret >= 0) {
        ret = KVDB_DBObject_delete_by_key(&DB, key); // del old entry
        if (ret < 0) {
            print_err_msg("KVDB_DBObject_delete_by_key failed\n");
            return -1;
        }
    }
    int h_idx = HASH_INDEX_LIB_HTObject_insert(DB.htObj, key_hash, i);
    if (h_idx < 0) {
        print_err_msg("HASH_INDEX_LIB_HTObject_insert failed\n");
        return -1;
    };

    DataEntryHeader RecordHeader = {0};
    RecordHeader.KeySize = key.len;
    RecordHeader.KeyType = key.type;
    RecordHeader.ValSize = val.len;
    RecordHeader.ValType = val.type;

    fseek(DB.fp, DB.OffsetPtr, SEEK_SET);
    fwrite(&RecordHeader, sizeof(RecordHeader), 1, DB.fp);
    fwrite(key.data, 1, key.len, DB.fp);

    DB.key_arr[i].len = key.len;
    DB.key_arr[i].type = key.type;
    DB.key_arr[i].data = (unsigned char*)malloc(key.len);
    if (!DB.key_arr[i].data) {
        print_err_msg("malloc(key.len) failed\n"); return -1;
    }
    memcpy(DB.key_arr[i].data, key.data, key.len);

    fwrite((unsigned char*)val.data, 1, (size_t)val.len, (FILE*)DB.fp);

    DB.IndexTable[i].KeyHash = fnv_1a_hash(key.data, key.len);
    DB.IndexTable[i].EntryID = i;
    DB.IndexTable[i].Flags |= FLAG_VALID;
    DB.IndexTable[i].Offset = DB.OffsetPtr;

    ulonglong_t DataEntrySize = sizeof(DataEntryHeader) + key.len + val.len;
    DB.OffsetPtr += DataEntrySize;
    DB.Header.ValidEntryCount++;
    DB.db_modified = 1;
    DB.Header.IndexTableOffset += DataEntrySize;
    return i++;
#undef i
}

int KVDB_DBObject_delete(DBObject* dbp, uint32_t EntryID) {
    if (EntryID >= DB.Header.EntryCount) {
        print_err_msg("Error: Invalid EntryID\n");
        return -1;
    }
    DB.db_modified = 1;

    int ret = HASH_INDEX_LIB_HTObject_delete(
        DB.htObj, DB.IndexTable[EntryID].KeyHash, EntryID
    );
    if (ret < 0) return -2;

    if (DB.key_arr[EntryID].data) {
        free(DB.key_arr[EntryID].data);
        DB.key_arr[EntryID].data = NULL;
    }
    DB.key_arr[EntryID].len = 0;
    DB.key_arr[EntryID].type = 0;

    DB.IndexTable[EntryID].KeyHash = 0;
    DB.IndexTable[EntryID].Flags = FLAG_DELETED;
    DB.IndexTable[EntryID].Offset = 0;

    DB.Header.ValidEntryCount--;
    return EntryID;
}

KVPair *KVDB_DBObject_get(DBObject* dbp, uint32_t EntryID) {
    if (EntryID >= DB.Header.EntryCapacity) {
        print_err_msg(ESC COLOUR_RED "Error: Invalid EntryID\n" ESC RESET_COLOUR);
        return NULL;
    }
    if (DB.IndexTable[EntryID].Flags & FLAG_DELETED) {
        print_err_msg(ESC COLOUR_RED "# Warning: Entry[%u] was deleted\n" ESC RESET_COLOUR, EntryID);
        return NULL;
    }

    fseek(DB.fp, DB.IndexTable[EntryID].Offset, SEEK_SET);
    DataEntryHeader RecordHeader = {0};
    size_t fread_cnt = fread(&RecordHeader, sizeof(DataEntryHeader), 1, DB.fp);
    if (fread_cnt != 1) {
        print_err_msg("[ERROR] fread(&RecordHeader, sizeof(DataEntryHeader), 1, DB.fp) != 1\n");
        return NULL;
    }
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
        goto KVDB_DBObject_get_failed_cleanup;
    }

    kv->key.len = RecordHeader.KeySize;
    kv->key.type = RecordHeader.KeyType;
    kv->val.len = RecordHeader.ValSize;
    kv->val.type = RecordHeader.ValType;

    kv->key.data = key;
    kv->val.data = val;

    fread_cnt = fread(key, RecordHeader.KeySize, 1, DB.fp);
    if (fread_cnt != 1) {
        print_err_msg("fread(key, RecordHeader.KeySize, 1, DB.fp) != 1\n");
        goto KVDB_DBObject_get_failed_cleanup;
    }
    fread_cnt = fread(val, RecordHeader.ValSize, 1, DB.fp);
    if (fread_cnt > 1) {
        print_err_msg("fread failed: fread_cnt=%zu\n", fread_cnt);
        goto KVDB_DBObject_get_failed_cleanup;
    }

    return kv;
KVDB_DBObject_get_failed_cleanup:
    if (key) free(key);
    if (val) free(val);
    free(kv);
    return NULL;
#undef MAX_VAL_SIZE
}

void KVDB_PrintKvPair(KVPair *kv) {
    if (!kv || !kv->key.data || !kv->val.data) return;

    putchar('\n');
    if (kv->key.type == TYPE_TEXT) {
        printf("key.data=\"%.*s\"\n", (int)kv->key.len, (char*)kv->key.data);
    } else {
        char* buffer = (char*)malloc(kv->key.len * 8ull + 256);
        if (!buffer) { perror("malloc failed"); return; }

        unsigned char* _key = (unsigned char*)kv->key.data;
        strcpy(buffer, "key.data=");
        append_str_hex(&buffer, _key, kv->key.len);
        fputs(buffer, stdout);

        free(buffer);
    }
#define MAX_VAL_SIZE 1024u
    if (!(kv->val.len < MAX_VAL_SIZE)) {
        printf(ESC COLOUR_YELLOW "# [WARN] val.data cannot be printed: val.len exceeds %u\n" ESC RESET_COLOUR, MAX_VAL_SIZE);
        return;
    }

    if (kv->val.type == TYPE_TEXT) {
        printf("val.data=\"%.*s\"\n", (int)kv->val.len, (char*)kv->val.data);
    } else {
        char* buffer = (char*)malloc(kv->val.len * 8ull + 256);
        if (!buffer) { perror("malloc failed"); return; }

        unsigned char* _val = (unsigned char*)kv->val.data;
        strcpy(buffer, "val.data=\\\n");
        append_str_hex(&buffer, _val, kv->val.len);
        strputc('\n', buffer);
        fputs(buffer, stdout);

        free(buffer);
    }
}

void KVDB_DestroyKVPair(KVPair *kv) {
    if (!kv) return;
    if (kv->key.data) free(kv->key.data);
    if (kv->val.data) free(kv->val.data);
    free(kv);
}


KVPair* KVDB_DBObject_get_by_key(DBObject* dbp, Key key) {
    int id = KVDB_conv_key_entry_id(&DB, key);
    if (id < 0) {
        print_err_msg(ESC COLOUR_RED "Error: Invalid key\n" ESC RESET_COLOUR); // Error
        return NULL;
    }
    KVPair *kv = KVDB_DBObject_get(&DB, (unsigned int)id);
    return kv;
}
int KVDB_DBObject_delete_by_key(DBObject* dbp, Key key) {
    int id = KVDB_conv_key_entry_id(&DB, key);
    if (id < 0) {
        print_err_msg(ESC COLOUR_RED "Error: Invalid key\n" ESC RESET_COLOUR); // Error
        return -1;
    }
    return KVDB_DBObject_delete(&DB, (ulong_t)id);
}

#undef DB