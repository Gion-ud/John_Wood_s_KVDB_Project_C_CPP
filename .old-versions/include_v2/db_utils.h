// db_utils.h

#ifndef DB_UTILS_H
#define DB_UTILS_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define PRINT_DBG_MSG(...) fprintf(stderr, __VA_ARGS__);fflush(stderr)

#ifdef _WIN32
    //#define fmemopen _fmemopen
#endif
#define EXIT_VOID           return

#define MEM_FILE_SIZE       (unsigned long long)(1024ULL * 1024ULL * 1024ULL * 4ULL)

#define BUFFER_SIZE         4096

#define FILE_PATH_SIZE      256
#define MAX_KEY_SIZE        256
#define MAGIC_SIZE          8
#define VERSION             0x02
#define HEADER_SIZE         sizeof(DBFileHeader)
#define INDEX_ENTRY_SIZE    sizeof(DBIndexEntry)

#pragma pack(push, 1)
enum EntryFlags {
    FLAG_UNUSED  = 0u,
    FLAG_VALID   = (1u << 0),
    FLAG_DELETED = (1u << 1),
};
enum TypeFlags {
    CHAR        = (1u << 0),
    INT         = (1u << 1),
    SHORT       = (1u << 2),
    LONG        = (1u << 3),
    FLOAT       = (1u << 4),
    DOUBLE      = (1u << 5),
    STRING      = (1u << 6),
    BLOB        = (1u << 7),
    RAW_BYTES   = (1u << 8)
};
enum ModTypeFlags {
    SIGNED      = (1u << 9),
    UNSIGNED    = (1u << 10),
};
typedef struct {
    uint8_t     Magic[MAGIC_SIZE];
    uint16_t    Version;
    uint64_t    HeaderSize;
    uint64_t    IndexEntrySize;
    uint32_t    EntryCount;
    uint32_t    ValidEntryCount;
    uint32_t    MaxEntryCount;
    uint64_t    DataSectionOffset;
} DBFileHeader;
typedef struct {
    uint32_t    size;
    uint32_t    type;
    void*       data;
} Key;
typedef struct {
    uint64_t    size;
    uint32_t    type;
    void*       data;
} Val;
typedef struct {
    uint32_t    ID;
    uint32_t    Flags;
    uint64_t    Size;
    uint64_t    Offset;
} DBIndexEntry;
typedef struct {
    uint32_t    KeySize;
    uint32_t    KeyType;
    uint64_t    ValSize;
    uint32_t    ValType;
} DataEntryHeader;
typedef struct {
    FILE*           fp;
    FILE*           fp_mem;
    void*           membuf;
    char*           filepath;
    DBFileHeader    Header;
    DBIndexEntry*   IndexTable;
    uint64_t        OffsetPtr;
} DBObject;
#pragma pack(pop)

#define ESC "\x1b"
//;[
#define COLOUR_BLACK   "[30m"
#define COLOUR_RED     "[31m"
#define COLOUR_GREEN   "[32m"
#define COLOUR_YELLOW  "[33m"
#define COLOUR_BLUE    "[34m"
#define COLOUR_MAGENTA "[35m"
#define COLOUR_CYAN    "[36m"
#define COLOUR_WHITE   "[37m"
#define RESET_COLOUR   "[0m"

#endif