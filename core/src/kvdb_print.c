#include <kvdb.h>
#include <kvdb_internal.h>

static char buf[BUFFER_SIZE] = {0};
static size32_t buf_cur = 0;
static const size32_t buf_size = BUFFER_SIZE;

void KVDB_DBObject_PrintFileHeader(int fd, DBObject *dbp) {
//  This is a helper function that prints the header
    if (fd == STDOUT_FILENO) { printf(ESC COLOUR_CYAN); }
    if (fd == STDERR_FILENO) { print_dbg_msg(ESC COLOUR_MAGENTA); }
    buf_cur = 0;

    const char *msg = "db.FileHeader.Magic=";
    size32_t msg_len = strlen(msg);
    memcpy((char*)buf + buf_cur, msg, msg_len);
    buf_cur += strlen(msg);

#define Header (dbp->Header)
    msg = (char*)conv_bytes_hex(Header.Magic, MAGIC_SIZE);
    msg_len = strlen(msg);
    memcpy((char*)buf + buf_cur, msg, msg_len);
    buf_cur += strlen(msg);

    char timestr[TIME_STR_SIZE] = {0};
    conv_time_str_modptr(&timestr, (qword_t)Header.LastModified);
    buf_cur += snprintf(
        (char*)buf + buf_cur,
        buf_size - buf_cur,
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
        (qword_t)Header.IndexTableOffset,
        (qword_t)Header.DataSectionOffset,
        (qword_t)Header.EOFHeaderOffset,
        timestr
    );
    write(fd, (char*)buf, buf_cur);
    buf_cur = 0;
    if (fd == STDOUT_FILENO) { printf(ESC RESET_COLOUR); }
    if (fd == STDERR_FILENO) { print_dbg_msg(ESC RESET_COLOUR); }
#undef Header
}
void KVDB_DBObject_PrintIndexEntry(int fd, DBObject *dbp, uint32_t EntryID) {
    if (fd == STDERR_FILENO) { print_dbg_msg(ESC COLOUR_CYAN); }
    //fprintf(fp, "# db.IndexTableEntry%.4u\n", EntryID);
    buf_cur = 0;
    buf_cur += snprintf(
        (char*)buf,
        BUFFER_SIZE,
        "db.index_entry%.4u.key_hash=0x%.16llx\n"
        "db.index_entry%.4u.flags=0x%.08x\n"
        "db.index_entry%.4u.entry_id=%.4u\n"
        "db.index_entry%.4u.offset=0x%.16llx\n\n",
        EntryID, (qword_t)dbp->IndexTable[EntryID].KeyHash,
        EntryID, dbp->IndexTable[EntryID].Flags,
        EntryID, dbp->IndexTable[EntryID].EntryID,
        EntryID, (qword_t)dbp->IndexTable[EntryID].Offset
    );
    write(fd, (char*)buf, buf_cur);
    buf_cur = 0;
    if (fd == STDERR_FILENO) { print_dbg_msg(ESC RESET_COLOUR); }
}
void KVDB_DBObject_PrintIndexTable(int fd, DBObject* dbp) {
    buf_cur = 0;
    const char *msg = "# IndexTable\n";
    size32_t msg_len = strlen(msg);
    write(fd, (char*)msg, msg_len);
    for (ulong_t i = 0; i < dbp->Header.EntryCount; i++) {
        KVDB_DBObject_PrintIndexEntry(fd, dbp, i);
    };
    buf_cur = 0;
}
void KVDB_DBObject_PrintRecordHeader(int fd, DBObject *dbp, uint32_t EntryID) {
    if (fd == STDERR_FILENO) { print_dbg_msg(ESC COLOUR_MAGENTA); }
    buf_cur = 0;
    buf_cur += snprintf(
        buf + buf_cur,
        buf_size - buf_cur,
        "# db.record%.4u\n",
        EntryID
    );
    DataEntryHeader RecordHeader = {0};
    fseek(dbp->fp, dbp->IndexTable[EntryID].Offset, SEEK_SET);
    size_t fread_ret = fread(&RecordHeader, sizeof(RecordHeader), 1, dbp->fp);
    if (fread_ret != 1) {
        print_err_msg("fread(&RecordHeader, sizeof(RecordHeader), 1, dbp->fp) failed\n");
        return;
    }
    buf_cur += snprintf(
        buf + buf_cur,
        buf_size - buf_cur,
        "db.record%.4u.key_len=%u\n"
        "db.record%.4u.key_type=0x%.08x\n"
        "db.record%.4u.val_len=%u\n"
        "db.record%.4u.val_type=0x%.08x\n",
        EntryID,RecordHeader.KeySize,
        EntryID,RecordHeader.KeyType,
        EntryID,RecordHeader.ValSize,
        EntryID,RecordHeader.ValType
    );
    write(fd, (char*)buf, buf_cur);
    buf_cur = 0;
    //write(fd, "something\n\n", 11);
    if (fd == STDERR_FILENO) { print_dbg_msg(ESC RESET_COLOUR); }
}
