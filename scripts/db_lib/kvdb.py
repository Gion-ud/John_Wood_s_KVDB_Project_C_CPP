#!/usr/bin/env python3

# scripts/db_lib_dll.py

import ctypes
from ctypes import *
# from ctypes import byref
import os

ROOT_DIR = os.path.dirname(__file__)
db_lib_dll_abspath = os.path.abspath(os.path.join(ROOT_DIR, "db_lib.dll"))
db_lib_dll = ctypes.CDLL(db_lib_dll_abspath)



class DBFileHeader(Structure):
	_pack_ = 1
	_fields_ = [
		("Magic", c_uint8 * 8),
		("Version", c_uint16),
		("ByteOrder", c_uint8),
		("HeaderSize", c_uint64),
		("IndexEntrySize", c_uint64),
		("EntryCount", c_uint32),
		("ValidEntryCount", c_uint32),
		("MaxEntryCount", c_uint32),
		("DataSectionOffset", c_uint64),
	]


class Key(Structure):
	_pack_ = 1
	_fields_ = [
		("size", c_uint32),
		("type", c_uint32),
		("data", c_void_p),
	]


class Val(Structure):
	_pack_ = 1
	_fields_ = [
		("size", c_uint64),
		("type", c_uint32),
		("data", c_void_p),
	]


class DBIndexEntry(Structure):
	_pack_ = 1
	_fields_ = [
		("ID", c_uint32),
		("Flags", c_uint32),
		("Size", c_uint64),
		("Offset", c_uint64),
	]


class DBObject(Structure):
	_pack_ = 1
	_fields_ = [
		("fp", c_void_p),
		("fp_hash", c_void_p),
		("key_arr", POINTER(Key)),
		("HashTable", POINTER(c_ulonglong)),
		("filepath", c_char_p),
		("Header", DBFileHeader),
		("IndexTable", POINTER(DBIndexEntry)),
		("OffsetPtr", c_uint64),
	]

db_lib_dll.DBInit.argtypes = [c_char_p, c_uint32]
db_lib_dll.DBInit.restype  = DBObject

db_lib_dll.DBOpen.argtypes = [c_char_p]
db_lib_dll.DBOpen.restype  = DBObject

db_lib_dll.WriteDBHeader.argtypes = [POINTER(DBObject)]
db_lib_dll.WriteDBHeader.restype  = None

db_lib_dll.CloseDB.argtypes = [POINTER(DBObject)]
db_lib_dll.CloseDB.restype  = None

db_lib_dll.InsertEntry.argtypes = [POINTER(DBObject), Key, Val]
db_lib_dll.InsertEntry.restype  = None

db_lib_dll.DeleteEntry.argtypes = [POINTER(DBObject), c_uint32]
db_lib_dll.DeleteEntry.restype  = None

db_lib_dll.db_key_to_id.argtypes = [POINTER(DBObject), Key, POINTER(c_uint32)]
db_lib_dll.db_key_to_id.restype  = ctypes.c_int

db_lib_dll.hash.argtypes = [ctypes.POINTER(ctypes.c_ubyte), c_size_t]
db_lib_dll.hash.restype  = c_ulonglong

db_lib_dll.ReadDBEntry.argtypes = [POINTER(DBObject), c_uint32]
db_lib_dll.ReadDBEntry.restype  = None

db_lib_dll.ReadDBEntryKey.argtypes = [POINTER(DBObject), Key]
db_lib_dll.ReadDBEntryKey.restype  = None

db_lib_dll.open_file_hash_table.argtypes = [POINTER(DBObject), c_char_p]
db_lib_dll.open_file_hash_table.restype  = c_int

db_lib_dll.init_file_hash_table.argtypes = [POINTER(DBObject), c_char_p]
db_lib_dll.init_file_hash_table.restype  = c_int

db_lib_dll.close_file_hash_table.argtypes = [POINTER(DBObject)]
db_lib_dll.close_file_hash_table.restype  = None

db_lib_dll.search_key.argtypes = [DBObject, Key]
db_lib_dll.search_key.restype  = c_long


FLAG_UNUSED = 0
FLAG_VALID = (1 << 0)
FLAG_DELETED = (1 << 1)

CHAR = (1 << 0)
INT = (1 << 1)
SHORT = (1 << 2)
LONG = (1 << 3)
FLOAT = (1 << 4)
DOUBLE = (1 << 5)
STRING = (1 << 6)
BLOB = (1 << 7)
RAW_BYTES = (1 << 8)

SIGNED = (1 << 9)
UNSIGNED = (1 << 10)


DBInit = db_lib_dll.DBInit
DBOpen = db_lib_dll.DBOpen
WriteDBHeader = db_lib_dll.WriteDBHeader
CloseDB = db_lib_dll.CloseDB
InsertEntry = db_lib_dll.InsertEntry
DeleteEntry = db_lib_dll.DeleteEntry
db_key_to_id = db_lib_dll.db_key_to_id
hash = db_lib_dll.hash
ReadDBEntry = db_lib_dll.ReadDBEntry
ReadDBEntryKey = db_lib_dll.ReadDBEntryKey
open_file_hash_table = db_lib_dll.open_file_hash_table
init_file_hash_table = db_lib_dll.init_file_hash_table
close_file_hash_table = db_lib_dll.close_file_hash_table
