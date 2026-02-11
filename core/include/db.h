// db_utils.h
#ifndef DB_H
#define DB_H

#include "global.h"
#include "hash_index_lib.h"
#include "typeflags.h"


#define MAX_KEY_SIZE            256
#define MAGIC_SIZE              8
#define VERSION                 0x05
#define HEADER_SIZE             sizeof(DBFileHeader)
#define INDEX_ENTRY_SIZE        sizeof(DBIndexEntry)
#define DATA_ENTRY_HEADER_SIZE  sizeof(DataEntryHeader)
#define HASH_SIZE               sizeof(hash_t)

#define INIT_INDEX_TABLE_CAP    32

typedef struct DBFileHeader DBFileHeader;
//typedef struct Key Key;
//typedef struct Val Val;
typedef struct _tlv_obj Key;
typedef struct _tlv_obj Val;
typedef struct _tlv_obj KV;

typedef struct DBIndexEntry DBIndexEntry;
typedef struct DataEntryHeader DataEntryHeader;
typedef struct DBObject DBObject;
typedef struct DBEOFHeader DBEOFHeader;

//typedef uint8_t  ubyte_t;
//typedef void*    ptr_t;

//typedef unsigned char pad_t;

enum EntryFlags {
    FLAG_UNUSED  = 0u,
    FLAG_VALID   = (1u << 0),
    FLAG_DELETED = (1u << 1),
};
/*
enum KVDBTypeFlags {
    CHAR        = (1u << 0),
    INT         = (1u << 1),
    SHORT       = (1u << 2),
    LONG        = (1u << 3),
    LONGLONG    = (1u << 4),
    FLOAT       = (1u << 5),
    DOUBLE      = (1u << 6),
    TEXT        = (1u << 7),
    BLOB        = (1u << 8),
    RAW_BYTES   = (1u << 9),
};
enum ModTypeFlags {
    SIGNED      = (1u << 9),
    UNSIGNED    = (1u << 10),
};
*/


#define LE 0

#pragma pack(push, 1)

struct DBFileHeader {
    ubyte_t     Magic[MAGIC_SIZE];  // 8 bytes
    uint16_t    Version;            // 2 bytes
    ubyte_t     ByteOrder;          // 1 byte
    size32_t    HeaderSize;         // 4 bytes
    size32_t    IndexEntrySize;     // 4 bytes
    ulong_t     EntryCapacity;      // 4 bytes
    ulong_t     EntryCount;         // 4 bytes
    ulong_t     ValidEntryCount;    // 4 bytes
    size32_t    DataEntryHeaderSize;// 4 bytes
    off64_t     IndexTableOffset;   // 8 bytes
    off64_t     DataSectionOffset;  // 8 bytes
    off64_t     EOFHeaderOffset;    // 8 bytes
    qword_t     LastModified;       // 8 bytes
    ubyte_t     Reserved[5];        // 5 bytes of padding...
};
struct DBIndexEntry {
    hash_t      KeyHash;    // 8 bytes
    ulong_t     EntryID;    // 4 bytes
    ulong_t     Flags;      // 4 bytes
    off64_t     Offset;     // 8 bytes
};
struct DataEntryHeader {
    size32_t    KeySize;    // 4 bytes
    uint32_t    KeyType;    // 4 bytes
    size32_t    ValSize;    // 4 bytes 
    uint32_t    ValType;    // 4 bytes
};
struct DBEOFHeader {
    ubyte_t     EOFMagic[MAGIC_SIZE];
};
#pragma pack(pop)
struct _tlv_obj {
    size32_t    len;
    ulong_t     type;
    void       *data;
};
typedef struct { Key key; Val val; } KVPair;

typedef struct _tlv_obj     TLVDataObject;
typedef struct _key_tbl_obj KeyTableObject;

struct DBObject {
    FILE           *fp;
    Key            *key_arr;
    HTObject       *htObj;
    DBFileHeader    Header;
    DBIndexEntry   *IndexTable;
    uint64_t        OffsetPtr;
    char            db_modified;    // 0 or 1
};

typedef struct KVDBEntry KVDBEntry;
struct KVDBEntry {
    DataEntryHeader Header;
    Key key;
    Val val;
};

#endif