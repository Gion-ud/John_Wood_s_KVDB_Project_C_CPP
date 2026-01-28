#!/usr/bin/env python3
from PIL import Image
import os
# import sys
import uuid
import ctypes
from ctypes import *

from db_lib import kvdb
db_lib = kvdb

ALLOC_BUF = ctypes.create_string_buffer
C_CAST = ctypes.cast

#Config
THUMBNAIL_SIZE=(64,64)
ROOT_DIR=b"c:/John_Wood/Coding_Projects/c/KVDB"
DIR_PATH=ROOT_DIR+b"/res/images"
EXPORT_PATH=ROOT_DIR+b"/thumbnails"
TXT_FILE_PATH=EXPORT_PATH+b"/thumbnames.txt"
EXT_ARR=(b".png",b".jpg",b".jpeg",b".bmp",b".webp")
#EndConfig

for filename in os.listdir(EXPORT_PATH):
	os.remove(os.path.join(EXPORT_PATH, filename))
#endfor

dir_ls = os.listdir(DIR_PATH)
dir_entry_count = len(dir_ls)
filenames = [None] * dir_entry_count
output_count = 0
for i in range(0, dir_entry_count):
	filename = dir_ls[i]
	if (filename.lower().endswith(EXT_ARR)):
		img_path = (DIR_PATH + b'/' + filename)
		img = Image.open(img_path)
		if (not img):
			print(f"Failed to open file \"{img_path}\"")
			continue
		#endif
		img.thumbnail(THUMBNAIL_SIZE)
		random_name = f"{uuid.uuid4().hex}.bmp".encode('ascii')
		img.save(EXPORT_PATH + b"/" + random_name)
		filenames[i] = (b"thumbnails/" + random_name)
		print("\x1b[32m" "Saved " f"{random_name}" "\x1b[0m")
		img.close()
		output_count += 1
	#endif
#endfor



MAX_ENTRY_COUNT = dir_entry_count
ENTRY_COUNT = dir_entry_count

db = db_lib.DBInit(b"database/thumbnails_0001.db", MAX_ENTRY_COUNT)
db_ptr = ctypes.pointer(db) 
db_lib.init_file_hash_table(db_ptr, b"database/thumbnails_0001_hash_table.mem")

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

db_lib.WriteDBHeader(db_ptr)
db_lib.close_file_hash_table(db_ptr)
db_lib.CloseDB(db_ptr)
