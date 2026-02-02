#ifndef KVDB_LIB_HPP
#define KVDB_LIB_HPP
extern "C" {
    #include "kvdb_lib.h"
};

namespace kvdb {
    class KVDBObj {
    private:
        DBObject* dbp;
    public:
        KVDBObj() { this->dbp = NULL; }
        void new_db(const char* filepath, uint32_t MaxEntryCount);
        void open_db(const char* filepath);
        void close_db();

        int get_key_id(Key key);

        int insert(Key key, Val val);
        void read_by_id(uint32_t EntryID);
        void delete_by_id(uint32_t EntryID);
        void read_by_key(Key key);
    };

    Key* set_key(const ubyte_t* key_data, size32_t key_size, uint32_t key_type);
    void destroy_key(Key* key);
    Val* set_val(const ubyte_t* val_data, size32_t val_size, uint32_t val_type);
    void destroy_val(Val* val);

    inline Key* set_key_str(const char* key_data) {
        return set_key((ubyte_t*)key_data, strlen((char*)key_data) + 1, STRING);
    }
    inline Val* set_val_str(const char* val_data) {
        return set_val((ubyte_t*)val_data, strlen((char*)val_data) + 1, STRING);
    }
}


#endif