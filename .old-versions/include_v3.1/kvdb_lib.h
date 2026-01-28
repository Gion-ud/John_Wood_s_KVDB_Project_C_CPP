#ifndef KVDB_LIB_H
#define KVDB_LIB_H

#include "db_lib.h"
#include "hash_table_lib.h"

DBObject* DBInit(const char* filepath, uint32_t MaxEntryCount);
DBObject* DBOpen(const char* filepath);
// FILE* init_hash_table(DBObject* dbp, const char* filepath);
// void close_hash_table(DBObject* dbp, FILE* fp_hash);
void WriteDBHeader(DBObject* dbp);
//void CommitDB(DBObject* dbp);
void CloseDB(DBObject* dbp);
int InsertEntry(DBObject* dbp, Key key, Val val);
void ReadDBEntry(DBObject* dbp, uint32_t EntryID);
void DeleteEntry(DBObject* dbp, uint32_t EntryID);
int db_key_to_id(DBObject* dbp, Key key, uint32_t* id);
void ReadDBEntryKey(DBObject* dbp, Key key);

int open_file_hash_table(DBObject* dbp, const char* filepath);
int init_file_hash_table(DBObject* dbp, const char* filepath);
void close_file_hash_table(DBObject* dbp);

hash_t kvdb_hash(const unsigned char* key, size_t len);
long int kvdb_search_key(DBObject db, Key key);


int InitHashTableObject(HashTableObject* pObj, int MaxEntryCount);
void DestroyHashTableObject(HashTableObject* pObj);
hidx_t insert_entry(HashTableObject* pObj, const byte_t* key, keysize_t key_size);
hidx_t search_key(HashTableObject* pObj, const byte_t* key, keysize_t key_size);
hidx_t delete_entry(HashTableObject* pObj, const byte_t* key, keysize_t key_size);

int kvdb_search_key_id(DBObject db, Key key);

#endif