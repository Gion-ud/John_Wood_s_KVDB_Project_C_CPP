// db_utils.h
#ifndef DB_UTILS_H
#define DB_UTILS_H

#include "global.h"
#include "hash_table_lib.h"


#define MAX_KEY_SIZE            256
#define MAGIC_SIZE              8
#define VERSION                 0x04
#define HEADER_SIZE             sizeof(DBFileHeader)
#define INDEX_ENTRY_SIZE        sizeof(DBIndexEntry)
#define DATA_ENTRY_HEADER_SIZE  sizeof(DataEntryHeader)
#define HASH_SIZE               sizeof(hash_t)

typedef struct DBFileHeader DBFileHeader;
typedef struct Key Key;
typedef struct Val Val;
typedef struct DBIndexEntry DBIndexEntry;
typedef struct DataEntryHeader DataEntryHeader;
typedef struct DBObject DBObject;

typedef uint64_t offptr_t;
typedef uint64_t _size64_t;
typedef uint32_t _size32_t;
typedef uint32_t count_t;
//typedef uint8_t  byte_t;
//typedef void*    ptr_t;

//typedef unsigned char pad_t;

typedef HashTableObject HTObject;

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
    RAW_BYTES   = (1u << 8),
};
enum ModTypeFlags {
    SIGNED      = (1u << 9),
    UNSIGNED    = (1u << 10),
};

#define LE 0

//#pragma pack(push, 1)
struct DBFileHeader {
    byte_t      Magic[MAGIC_SIZE];  // 8 bytes

    uint16_t    Version;            // 2 bytes
    byte_t      _pad0[5];           // 5 bytes of padding...
    byte_t      ByteOrder;          // 1 byte

    _size64_t   HeaderSize;         // 8 bytes
    _size64_t   IndexEntrySize;     // 8 bytes

    count_t     EntryCount;         // 4 bytes
    count_t     ValidEntryCount;    // 4 bytes

    count_t     MaxEntryCount;      // 4 bytes
    byte_t      _pad1[4];           // 4 bytes of padding...

    offptr_t    DataSectionOffset;  // 8 bytes
    _size64_t   DataEntryHeaderSize;// 8 bytes
};
struct DBIndexEntry {
    hash_t      KeyHash;    // 8 bytes

    uint32_t    Flags;      // 4 bytes
    byte_t      _pad0[4];   // 4 bytes of padding...

    _size64_t   Size;       // 8 bytes
    offptr_t    Offset;     // 8 bytes
};
struct DataEntryHeader {
    _size32_t   KeySize;    // 4 bytes
    uint32_t    KeyType;    // 4 bytes
    offptr_t    KeyOffset;  // 8 bytes

    _size32_t   ValSize;    // 4 bytes 
    uint32_t    ValType;    // 4 bytes
    offptr_t    ValOffset;  // 8 bytes
};
//#pragma pack(pop)
struct Key {
    _size32_t   size;
    uint32_t    type;
    ptr_t       data;
};
struct Val {
    _size64_t   size;
    uint32_t    type;
    ptr_t       data;
};
struct DBObject {
    FILE*           fp;
    FILE*           fp_hash; // fp for hash_table on disk
    Key*            key_arr;
    int*        EntryID_arr_htidx;
    HTObject*       htObj;
//  FILE*           fp_mem;
//  void*           membuf;
    hash_t*         HashTable;  // Old hash arr
    char*           filepath;
    DBFileHeader    Header;
    DBIndexEntry*   IndexTable;
    uint64_t        OffsetPtr;
};

#endif