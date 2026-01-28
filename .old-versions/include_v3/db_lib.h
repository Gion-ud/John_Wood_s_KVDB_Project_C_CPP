// db_lib.h with APIs
#ifndef DB_LIB_H
#define DB_LIB_H

#include "../include/db_utils.h"

// uint64_t GetDataEntrySize(uint32_t KeySize, uint64_t ValSize);
// uint64_t GetIndexEntryOffset(DBObject db, uint32_t EntryID);
DBObject DBInit(const char* filepath, uint32_t MaxEntryCount);
DBObject DBOpen(const char* filepath);
// FILE* init_hash_table(DBObject* dbp, const char* filepath);
// void close_hash_table(DBObject* dbp, FILE* fp_hash);
void WriteDBHeader(DBObject* dbp);
//void CommitDB(DBObject* dbp);
void CloseDB(DBObject* dbp);
void InsertEntry(DBObject* dbp, Key key, Val val);
void ReadDBEntry(DBObject* dbp, uint32_t EntryID);
void DeleteEntry(DBObject* dbp, uint32_t EntryID);
int db_key_to_id(DBObject* dbp, Key key, uint32_t* id);
void ReadDBEntryKey(DBObject* dbp, Key key);

int open_file_hash_table(DBObject* dbp, const char* filepath);
int init_file_hash_table(DBObject* dbp, const char* filepath);
void close_file_hash_table(DBObject* dbp);


hash_t hash(const unsigned char* key, size_t len);
long int search_key(DBObject db, Key key);

#endif