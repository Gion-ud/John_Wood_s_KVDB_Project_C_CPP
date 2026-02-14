# kvdb.pxd

cdef extern from "kvdb.h":
    ctypedef unsigned int size32_t
    ctypedef unsigned long ulong_t
    ctypedef unsigned int uint32_t

    ctypedef struct _tlv_obj:
        size32_t len
        ulong_t type
        void* data

    ctypedef _tlv_obj Key
    ctypedef _tlv_obj Val
    ctypedef _tlv_obj TLVDataObject

    ctypedef struct KVPair:
        Key key
        Val val

    ctypedef struct DBObject:
        pass

    DBObject* KVDB_DBObject_create(const char* filepath, int EntryCapacity)
    DBObject* KVDB_DBObject_open(const char* filepath)
    void KVDB_DBObject_close(DBObject* dbp)
    ulong_t KVDB_DBObject_EntryCount(DBObject* dbp)
    KVPair* KVDB_DBObject_get(DBObject* dbp, uint32_t EntryID)
    int KVDB_DBObject_put(DBObject* dbp, Key key, Val val)
    int KVDB_DBObject_delete(DBObject* dbp, uint32_t EntryID)
    void KVDB_PrintKvPair(KVPair* kv)
    void KVDB_DestroyKVPair(KVPair* kv)
    KVPair* KVDB_DBObject_get_by_key(DBObject* dbp, Key key)
    int KVDB_DBObject_delete_by_key(DBObject* dbp, Key key)

