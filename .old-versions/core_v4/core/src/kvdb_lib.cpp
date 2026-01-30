#include "kvdb_lib.hpp"

void kvdb::KVDBObj::new_db(const char* filepath, uint32_t MaxEntryCount) {
    this->dbp = DBInit(filepath, MaxEntryCount);
}
void kvdb::KVDBObj::open_db(const char* filepath) {
    this->dbp = DBOpen(filepath);
}
void kvdb::KVDBObj::close_db() {
    CloseDB(this->dbp);
}
int kvdb::KVDBObj::get_key_id(Key key) {
    return kvdb_search_key_id(this->dbp, key);
}
int kvdb::KVDBObj::insert(Key key, Val val) {
    return InsertEntry(this->dbp, key, val);
}
void kvdb::KVDBObj::read_by_id(uint32_t EntryID) {
    ReadDBEntry(this->dbp, EntryID);
}
void kvdb::KVDBObj::delete_by_id(uint32_t EntryID) {
    //if (EntryID >= this->dbp->Header.EntryCount) return; // This Guard is crucial
    DeleteEntry(this->dbp, EntryID);
}
void kvdb::KVDBObj::read_by_key(Key key) {
    ReadDBEntryKey(this->dbp, key);
}

Key* kvdb::set_key(const ubyte_t* key_data, size32_t key_size, uint32_t key_type) {
    Key *key = (Key*)malloc(sizeof(Key));
    if (!key) { perror("malloc"); return nullptr; }
    key->data = (ubyte_t*)malloc(key_size);
    if (!key->data) { perror("malloc"); free(key); return nullptr; }
    memcpy((ubyte_t*)key->data, key_data, key_size);
    key->type = key_type;
    key->size = key_size;
    return key;
}
void kvdb::destroy_key(Key* key) {
    if (key->data) free(key->data);
    key->data = NULL;
    key->size = 0;
    key->type = 0;
    free(key);
}

Val* kvdb::set_val(const ubyte_t* val_data, size32_t val_size, uint32_t val_type) {
    Val *val = (Val*)malloc(sizeof(Val));
    if (!val) { perror("malloc"); return nullptr; }
    val->data = (ubyte_t*)malloc(val_size);
    if (!val->data) { perror("malloc"); return nullptr; }
    memcpy((ubyte_t*)val->data, val_data, val_size);
    val->type = val_type;
    val->size = val_size;
    return val;
}
void kvdb::destroy_val(Val* val) {
    if (val->data) free(val->data);
    val->data = NULL;
    val->size = 0;
    val->type = 0;
    free(val);
}