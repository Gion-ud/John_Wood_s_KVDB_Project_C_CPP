#!/usr/bin/env python3
# scripts/mkdb.py

from db_lib import kvdb
db_lib = kvdb
import ctypes
from ctypes import *

alloc_buf = ctypes.create_string_buffer
c_cast = ctypes.cast

filenames = [
	b"thumbnails/477e16d1592c4a4e9252387ef88334d0.bmp",
	b"thumbnails/8998e88eac7b4e02bea30c73302a275a.bmp",
	b"thumbnails/e3453b6aa95541c2bea162ecd659c2e8.bmp",
	b"thumbnails/dcd79c6445fe4fe389283c08d1efd2be.bmp",
	b"thumbnails/a13ac8ec11b2459aa257132bf38a23ef.bmp",
	b"thumbnails/3f17f3e8889d4f078311d231f7046cb7.bmp",
	b"thumbnails/9117fa64d7eb4d679ce57a25643d20a4.bmp",
	b"thumbnails/7a4b4fe09d534a1aaf6b00ba5ad548ac.bmp",
	b"thumbnails/e4cf208f16ed434d8a9790f1ba65bf19.bmp",
	b"thumbnails/78a4d942eb4f4569bf6203a9750e7af0.bmp",
	b"thumbnails/eb02cf4197754c1ba62741f8d155b310.bmp",
	b"thumbnails/96c9275c82c5462c86ca034fc6b990af.bmp",
	b"thumbnails/08693d8c42e749e380113efcf6432469.bmp",
	b"thumbnails/fe783b26d5ae40378ebea8fac08571d0.bmp",
	b"thumbnails/9e31411754c249d4baa6007ccb68229b.bmp",
	b"thumbnails/68a76808ec864d7fb0848af687b2de0e.bmp",
	b"thumbnails/c86274dc67424afb865f682900a38d67.bmp",
	b"thumbnails/d4af5f2dc7be45e69c666f1956e04869.bmp",
	b"thumbnails/71ad27cdd4474bffa877b612a4df90eb.bmp",
	b"thumbnails/b13d2c2a88384c24874c27dd63716a56.bmp",
	b"thumbnails/57736d5905924222985b43cc881cb571.bmp",
	b"thumbnails/04e5fffd7c604c738434d73d5ccae479.bmp",
]

MAX_ENTRY_COUNT = 32
ENTRY_COUNT = len(filenames)

db = db_lib.DBInit(b"database/thumbnails_0001.db", MAX_ENTRY_COUNT)
db_ptr = ctypes.pointer(db) 
db_lib.init_file_hash_table(db_ptr, b"database/thumbnails_0001_hash_table.mem")

key = [None] * ENTRY_COUNT
val = [None] * ENTRY_COUNT

for i in range(0, ENTRY_COUNT):
	key[i] = db_lib.Key()
	key[i].size = len(filenames[i]) + 1
	key[i].type = db_lib.STRING
	key_buf = alloc_buf(filenames[i], key[i].size)
	key[i].data = c_cast(key_buf, c_void_p)

	fref = open(filenames[i], "rb")
	data = fref.read()
	fref.close()

	val_buf = alloc_buf(data, len(data))

	val[i] = db_lib.Val()
	val[i].size = len(data)
	val[i].type = db_lib.BLOB
	val[i].data = c_cast(val_buf, c_void_p)

	db_lib.InsertEntry(db_ptr, key[i], val[i])
#endfor

db_lib.WriteDBHeader(db_ptr)
db_lib.close_file_hash_table(db_ptr)
db_lib.CloseDB(db_ptr)
