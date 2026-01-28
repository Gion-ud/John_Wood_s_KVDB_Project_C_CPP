#ifndef DB_LIB_H
#define DB_LIB_H

#include "../include/db_utils.h"

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
void ReadDBEntryKey(DBObject* dbp, Key key);

#endif