# kvdb.pyx
from libc.stdlib cimport malloc, free
from libc.string cimport memcpy
from kvdb cimport *

cdef class PyTLV:
    cdef TLVDataObject tlv

    def __init__(self):
        self.tlv.len = 0
        self.tlv.type = 0
        self.tlv.data = NULL

    def new(self, unsigned char* data, unsigned int data_len, unsigned int dat_type):
        if (self.tlv.data):
            free(self.tlv.data)

        self.tlv.len = data_len
        self.tlv.type = dat_type
        self.tlv.data = malloc(data_len)
        if (not self.tlv.data):
            raise RuntimeError("malloc failed")

        memcpy(self.tlv.data, data, data_len)

    def delete(self):
        if (self.tlv.data):
            free(self.tlv.data)
            self.tlv.data = NULL

    def __dealloc__(self):
        if (self.tlv.data):
            free(self.tlv.data)
            self.tlv.data = NULL

    cdef TLVDataObject this(self):
        return self.tlv


cdef class PyKVDB:
    cdef DBObject* db

    def __cinit__(self):
        self.db = NULL

    def create(self, path, cap):
        cdef bytes bpath = path.encode('ascii')
        self.db = KVDB_DBObject_create(bpath, cap)
        if (not self.db):
            raise RuntimeError("KVDB_DBObject_create failed")

    def open(self, path):
        cdef bytes bpath = path.encode('ascii')
        self.db = KVDB_DBObject_open(bpath)
        if (not self.db):
            raise RuntimeError("KVDB_DBObject_open failed")

    def close(self):
        if (self.db):
            KVDB_DBObject_close(self.db)
            self.db = NULL

    def __dealloc__(self):
        self.close()

    def put(self, PyTLV key, PyTLV val):
        return KVDB_DBObject_put(self.db, key.this(), val.this())

    def get(self, int entry_id):
        cdef KVPair* kv = KVDB_DBObject_get(self.db, entry_id)
        if (not kv):
            return None
        pykey = bytes(<char*>kv.key.data, kv.key.len)
        pyval = bytes(<char*>kv.val.data, kv.val.len)
        KVDB_DestroyKVPair(kv)
        return pykey, pyval
