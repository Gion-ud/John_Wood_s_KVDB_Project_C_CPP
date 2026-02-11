// db_lib.h with APIs
#ifndef DB_LIB_H
#define DB_LIB_H

#include "db.h"

int kvdb_search_key_id(DBObject* dbp, Key key);

static inline void PrintDBFileHeader(FILE* fp, DBObject *dbp) {
//  This is a helper function that prints the header
    if (fp == stdout) { printf(ESC COLOUR_CYAN); }
    if (fp == stderr) { print_dbg_msg(ESC COLOUR_MAGENTA); }

    fputs("# db.FileHeader\n", fp);
    fputs("db.FileHeader.Magic=", fp);
#define Header (dbp->Header)
{
    char* tmp = (char*)conv_bytes_hex(Header.Magic, MAGIC_SIZE);
    fputs(tmp, fp);
    free(tmp);
}
    char timestr[TIME_STR_SIZE] = {0};
    conv_time_str_modptr(&timestr, (uqword_t)Header.LastModified);
    fprintf(
        fp,
        "db.FileHeader.Version=%u\n"
        "db.FileHeader.ByteOrder=0x%.02x\n"
        "db.FileHeader.HeaderSize=%u\n"
        "db.FileHeader.IndexEntrySize=%u\n"
        "db.FileHeader.EntryCapacity=%u\n"
        "db.FileHeader.EntryCount=%u\n"
        "db.FileHeader.ValidEntryCount=%u\n"
        "db.FileHeader.DataEntryHeaderSize=%u\n"
        "db.FileHeader.IndexTableOffset=0x%.16llx\n"
        "db.FileHeader.DataSectionOffset=0x%.16llx\n"
        "db.FileHeader.EOFHeaderOffset=0x%.16llx\n"
        "db.FileHeader.LastModified=%s\n"
        "\n",
        Header.Version,
        Header.ByteOrder,
        Header.HeaderSize,
        Header.IndexEntrySize,
        Header.EntryCapacity,
        Header.EntryCount,
        Header.ValidEntryCount,
        Header.DataEntryHeaderSize,
        (uqword_t)Header.IndexTableOffset,
        (uqword_t)Header.DataSectionOffset,
        (uqword_t)Header.EOFHeaderOffset,
        timestr
    );
    if (fp == stdout) { printf(ESC RESET_COLOUR); }
    if (fp == stderr) { print_dbg_msg(ESC RESET_COLOUR); }
#undef Header
}
static inline void PrintIndexEntry(FILE* fp, DBObject *dbp, uint32_t EntryID) {
    if (fp == stderr) { PRINT_DBG_MSG(ESC COLOUR_CYAN); }
    fprintf(fp, "# db.IndexTableEntry%.4u\n", EntryID);
    fprintf(
        fp,
        "db.IndexTableEntry%.4u.KeyHash=0x%.16llx\n"
        "db.IndexTableEntry%.4u.Flags=0x%.08x\n"
        "db.IndexTableEntry%.4u.EntryID=%.4u\n"
        "db.IndexTableEntry%.4u.Offset=0x%.16llx\n\n",
        EntryID, (uqword_t)dbp->IndexTable[EntryID].KeyHash,
        EntryID, dbp->IndexTable[EntryID].Flags,
        EntryID, dbp->IndexTable[EntryID].EntryID,
        EntryID, (uqword_t)dbp->IndexTable[EntryID].Offset
    );
    if (fp == stderr) { PRINT_DBG_MSG(ESC RESET_COLOUR); }
}
static inline void PrintIndexTable(FILE* fp, DBObject* dbp) {
    fputs("[IndexTable]\n", fp);
    //fseek(db.fp, db.Header.HeaderSize, SEEK_SET);
    for (ulong_t i = 0; i < dbp->Header.EntryCount; i++) {
        //fread(&db.IndexTable[i], INDEX_ENTRY_SIZE, 1, db.fp);
        PrintIndexEntry(fp, dbp, i);
    };
}
static inline void PrintRecordHeader(FILE* fp, DBObject *dbp, uint32_t EntryID) {
    if (fp == stderr) { PRINT_DBG_MSG(ESC COLOUR_MAGENTA); }
    fprintf(fp, "# db.record%.4u\n", EntryID);
    DataEntryHeader RecordHeader = {0};
    fseek(dbp->fp, dbp->IndexTable[EntryID].Offset, SEEK_SET);
    size_t fread_ret = fread(&RecordHeader, sizeof(RecordHeader), 1, dbp->fp);
    if (fread_ret != 1) {
        print_err_msg("fread(&RecordHeader, sizeof(RecordHeader), 1, dbp->fp) failed\n");
        return;
    }
    fprintf(
        fp,
        "db.record%.4u.key_len=%u\n"
        "db.record%.4u.key_type=0x%.08x\n"
        "db.record%.4u.val_len=%u\n"
        "db.record%.4u.val_type=0x%.08x\n",
        EntryID,RecordHeader.KeySize,
        EntryID,RecordHeader.KeyType,
        EntryID,RecordHeader.ValSize,
        EntryID,RecordHeader.ValType
    );
    if (fp == stderr) { PRINT_DBG_MSG(ESC RESET_COLOUR); }
}


int KVDB_conv_key_entry_id(DBObject* dbp, Key key);
DBObject* KVDB_DBObject_create(const char* filepath, int EntryCapacity);
DBObject* KVDB_DBObject_open(const char* filepath);
void KVDB_DBObject_close(DBObject* dbp);
int KVDB_DBObject_insert(DBObject* dbp, Key key, Val val);
int KVDB_DBObject_delete(DBObject* dbp, uint32_t EntryID);
KVPair *KVDB_DBObject_get(DBObject* dbp, uint32_t EntryID);
void KVDB_PrintKvPair(KVPair *kv);
void KVDB_DestroyKVPair(KVPair *kv);
KVPair* KVDB_DBObject_get_by_key(DBObject* dbp, Key key);
int KVDB_DBObject_delete_by_key(DBObject* dbp, Key key);
#endif