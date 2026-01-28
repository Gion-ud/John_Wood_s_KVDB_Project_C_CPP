// Ignore this shit for now
#ifndef KVDB_LIB_HPP
#define KVDB_LIB_HPP

extern "C" {
    #include "../include/db_lib.h"
}

// uint64_t GetDataEntrySize(uint32_t KeySize, uint64_t ValSize);
// uint64_t GetIndexEntryOffset(DBObject db, uint32_t EntryID);
DBObject DBInit(const char* filepath, uint32_t MaxEntryCount);
void WriteDBHeader(DBObject* dbp);
//void CommitDB(DBObject* dbp);
void CloseDB(DBObject* dbp);
void InsertEntry(DBObject* dbp, Key key, Val val);
void ReadDBEntry(DBObject* dbp, uint32_t EntryID);
void DeleteEntry(DBObject* dbp, uint32_t EntryID);
DBObject DBOpen(const char* filepath);
int db_key_to_id(DBObject* dbp, Key key, uint32_t* id);
void ReadDBEntryKey(DBObject* dbp, Key key);

#define NEW_DB 0
#define OPEN_DB 1

namespace KVDB {

    class DB {
    //#define db (*dbp)
    private:
        //DBObject* dbp;
        DBObject db;
    public:
        DB(int open_num, const char* filename, uint32_t MaxEntryCount) {
            if (open_num == NEW_DB) {
                db = DBInit(filename, MaxEntryCount);
            } else {
                return;
            }
        }
        DB(int open_num, const char* filename) {
            if (open_num == OPEN_DB) {
                db = DBOpen(filename);
            } else {
                return;
            }
        }
        void new_db(const char* filename, uint32_t MaxEntryCount) {
            db = DBInit(filename, MaxEntryCount);
        }
        void open_db(const char* filename) {
            db = DBOpen(filename);
        }
        void insert_entry(Key key, Val val) {
            InsertEntry(&db, key, val);
        }
        void delete_entry(uint32_t EntryID) {
            DeleteEntry(&db, EntryID);
        }
        void read_entry(uint32_t EntryID) {
            ReadDBEntry(&db, EntryID);
        }
        void conv_key_id(Key key, uint32_t* id) {
            db_key_to_id(&db, key, id);
        }
        void read_entry_key(Key key) {
            ReadDBEntryKey(&db, key);
        }
        void write_header() {
            WriteDBHeader(&db);
        }
        ~DB() {
            CloseDB(&db);
            //free(dbp);
        }
    //#undef db
    };

}

#endif