#!/usr/bin/env python3
import os
import ctypes
from ctypes import *
from db_lib import kvdb

db_lib = kvdb

ALLOC_BUF = ctypes.create_string_buffer
C_CAST = ctypes.cast

#Config
ROOT_DIR=b"c:/John_Wood/Coding_Projects/c/KVDB"
DIR_PATH=ROOT_DIR+b"/thumbnails"
#EndConfig


dir_ls = os.listdir(DIR_PATH)
dir_entry_count = len(dir_ls)
filenames = [None] * dir_entry_count
for i in range(0, dir_entry_count):
	filename = dir_ls[i]
#endfor

ENTRY_COUNT = dir_entry_count

db = db_lib.DBOpen(b"database/thumbnails_0001.db")
db_ptr = ctypes.pointer(db) 
db_lib.open_file_hash_table_file_hash_table(db_ptr, b"database/thumbnails_0001_hash_table.mem")

key = [None] * ENTRY_COUNT
val = [None] * ENTRY_COUNT

for i in range(0, ENTRY_COUNT):
	key[i] = db_lib.Key(), val[i] = db_lib.Val()

	key[i].size = len(filenames[i]) + 1
	key[i].type = db_lib.STRING
	key_buf = ALLOC_BUF(filenames[i], key[i].size)
	key[i].data = C_CAST(key_buf, c_void_p)

	fref = open(filenames[i], "rb")
	data = fref.read()
	fref.close()

	val_buf = ALLOC_BUF(data, len(data))

	val[i].size = len(data)
	val[i].type = db_lib.BLOB
	val[i].data = C_CAST(val_buf, c_void_p)

	db_lib.InsertEntry(db_ptr, key[i], val[i])
#endfor
db_lib.close_file_hash_table(db_ptr)
db_lib.CloseDB(db_ptr)
