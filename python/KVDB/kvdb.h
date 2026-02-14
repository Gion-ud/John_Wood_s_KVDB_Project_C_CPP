// kvdb.h
#ifndef KVDB_H
#define KVDB_H

#include "global.h"
#include "typeflags.h"


#ifdef _WIN32
    #ifdef DLL_EXPORT
        #define KVDB_API __declspec(dllexport)
    #else
        #define KVDB_API __declspec(dllimport)
    #endif
#else
    #define KVDB_API __attribute__((visibility("default")))
#endif

typedef struct _tlv_obj Key;
typedef struct _tlv_obj Val;
typedef struct _tlv_obj TLVDataObject;
struct _tlv_obj {
    size32_t    len;
    ulong_t     type;
    void       *data;
};

typedef struct { Key key; Val val; } KVPair;

typedef struct DBObject DBObject;

//int KVDB_conv_key_entry_id(DBObject* dbp, Key key);
KVDB_API DBObject* KVDB_DBObject_create(const char* filepath, int EntryCapacity);
KVDB_API DBObject* KVDB_DBObject_open(const char* filepath);
KVDB_API void KVDB_DBObject_close(DBObject* dbp);
KVDB_API ulong_t KVDB_DBObject_EntryCount(DBObject* dbp);
KVDB_API KVPair *KVDB_DBObject_get(DBObject* dbp, uint32_t EntryID);
KVDB_API int KVDB_DBObject_put(DBObject* dbp, Key key, Val val);
KVDB_API int KVDB_DBObject_delete(DBObject* dbp, uint32_t EntryID);
KVDB_API void KVDB_PrintKvPair(KVPair *kv);
KVDB_API void KVDB_DestroyKVPair(KVPair *kv);
KVDB_API KVPair* KVDB_DBObject_get_by_key(DBObject* dbp, Key key);
KVDB_API int KVDB_DBObject_delete_by_key(DBObject* dbp, Key key);

KVDB_API void KVDB_DBObject_PrintFileHeader(FILE* fp, DBObject *dbp);
KVDB_API void KVDB_DBObject_PrintIndexEntry(FILE* fp, DBObject *dbp, uint32_t EntryID);
KVDB_API void KVDB_DBObject_PrintIndexTable(FILE* fp, DBObject* dbp);
KVDB_API void KVDB_DBObject_PrintRecordHeader(FILE* fp, DBObject *dbp, uint32_t EntryID);


KVDB_API void KVDB_DBObject_PrintFileHeader_stdout(DBObject *dbp);
KVDB_API void KVDB_DBObject_PrintIndexEntry_stdout(DBObject *dbp, uint32_t EntryID);
KVDB_API void KVDB_DBObject_PrintIndexTable_stdout(DBObject* dbp);
KVDB_API void KVDB_DBObject_PrintRecordHeader_stdout(DBObject *dbp, uint32_t EntryID);


#endif