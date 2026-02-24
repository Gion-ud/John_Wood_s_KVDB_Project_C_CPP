// kvdb.h
#ifndef KVDB_H
#define KVDB_H

#include "global.h"
#include "typeflags.h"

#if defined(_MSC_VER)
    #ifdef DLL_EXPORT
        #define KVDB_API __declspec(dllexport)
    #else
        #define KVDB_API __declspec(dllimport)
    #endif
#elif defined(__GNUC__) || defined(__clang__) || defined(__ICC)
    #define KVDB_API __attribute__((visibility("default")))
#else
    #define KVDB_API
#endif

typedef struct _tlv_obj Key;
typedef struct _tlv_obj Val;
typedef struct _tlv_obj TLVDataObject;
struct _tlv_obj {
    ulong_t     type;
    size32_t    len;
    void       *data;
};

typedef struct { Key key; Val val; } KVPair;

typedef struct DBObject DBObject;

//int KVDB_conv_key_entry_id(DBObject* dbp, Key key);
KVDB_API DBObject *KVDB_DBObject_create(const char* filepath, int EntryCapacity);
KVDB_API DBObject *KVDB_DBObject_open(const char* filepath);
KVDB_API void KVDB_DBObject_close(DBObject* dbp);
KVDB_API ulong_t KVDB_DBObject_EntryCount(DBObject* dbp);
KVDB_API Val *KVDB_DBObject_get(DBObject* dbp, uint32_t EntryID);
KVDB_API Key *KVDB_DBObject_get_key(DBObject* dbp, uint32_t EntryID);
KVDB_API int KVDB_DBObject_put(DBObject* dbp, const Key* key, const Val* val);
KVDB_API Val *KVDB_DBObject_get_by_key(DBObject* dbp, const Key* key);
KVDB_API int KVDB_DBObject_delete_by_key(DBObject* dbp, const Key* key);
KVDB_API int KVDB_DBObject_delete(DBObject* dbp, uint32_t EntryID);


KVDB_API void KVDB_TLVDataObject_print(TLVDataObject *tlv);
KVDB_API void KVDB_TLVDataObject_destroy(TLVDataObject *tlv);


KVDB_API void KVDB_DBObject_PrintFileHeader(int fd, DBObject *dbp);
KVDB_API void KVDB_DBObject_PrintIndexEntry(int fd, DBObject *dbp, uint32_t EntryID);
KVDB_API void KVDB_DBObject_PrintIndexTable(int fd, DBObject* dbp);
KVDB_API void KVDB_DBObject_PrintRecordHeader(int fd, DBObject *dbp, uint32_t EntryID);



#endif