#ifndef KVDB_HPP
#define KVDB_HPP

extern "C" {
    #include <kvdb.h>
    #include <kvdb_internal.h>
}
#include <iostream>

class KVDB {
private:
    DBObject *dbp;
public:
    KVDB() {
        dbp = nullptr;
    }
    bool create(const char *filepath, int EntryCapacity) {
        this->dbp = KVDB_DBObject_create(filepath, EntryCapacity);
        if (!this->dbp) {
            std::cerr << "KVDB::create failed\n";
            return false;
        }
        return true;
    }
    bool open(const char *filepath) {
        this->dbp = KVDB_DBObject_open(filepath);
        if (!this->dbp) {
            std::cerr << "KVDB::open failed\n";
            return false;
        }
        return true;
    }
    int put(Key& key, Val& val) {
        if (!this->dbp) return -1;
        int ret = KVDB_DBObject_put(this->dbp, &key, &val);
        if (ret < 0) {
            std::cerr << "KVDB::put failed\n";
            return -1;
        }
        return ret;
    }
    Val *get(Key& key) {
        if (!this->dbp) return nullptr;
        Val *val = KVDB_DBObject_get_by_key(this->dbp, &key);
        if (!val) {
            std::cerr << "KVDB::get failed\n";
            return nullptr;
        }
        return val;
    }
    void destroy_val_p(Val *val) {
        if (!val) return;
        KVDB_TLVDataObject_destroy(val);
    }
    DBObject *get_this() {
        return this->dbp;
    }
    ~KVDB() {
        if (this->dbp) {
            KVDB_DBObject_close(this->dbp);
            this->dbp = nullptr;
        }
    }
};

#endif