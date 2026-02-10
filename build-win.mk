build-bin: bin/kvdb_lib.dll mkdb_ldll rddb_ldll get_db_rec_ldll

MODULE_OBJ = build/db_lib.o build/hash_func_module.o build/hash_index_lib.o build/global_func.o

# dll for kvdb
bin/kvdb_lib.dll: $(MODULE_OBJ) core/def/kvdb_lib.def | build core/src core/lib core/include core/def bin
	gcc -O2 -s -shared \
		-I./core/include \
		$(MODULE_OBJ) core/def/kvdb_lib.def \
		-o bin/kvdb_lib.dll \
		-Wl,--kill-at \
		-Wl,--out-implib,core/lib/libkvdb_lib.dll.a
#		-Wl,--output-def,core/lib/kvdb_lib_exports.def
#	dlltool -D $@ -d core/def/kvdb_lib.def -l core/lib/libkvdb_lib.dll.a


mkdb_ldll: tests/mkdb.c bin/kvdb_lib.dll build/txt_tok_lib.o build/global_func.o | core/include core/src bin tests
	gcc tests/mkdb.c build/txt_tok_lib.o build/global_func.o \
		-O2 -s -I./core/include -Lbin -lkvdb_lib \
		-Wl,-rpath=./bin -o bin/mkdb
# -g -fsanitize=address

rddb_ldll: tests/rddb.c bin/kvdb_lib.dll build/txt_tok_lib.o build/global_func.o | core/include core/src bin tests
	gcc tests/rddb.c build/txt_tok_lib.o build/global_func.o \
		-O2 -s -I./core/include -Lbin -lkvdb_lib \
		-Wl,-rpath=./bin -o bin/rddb

get_db_rec_ldll: tests/getdbrec.c bin/kvdb_lib.dll | core/include core/src core/lib bin tests
	gcc tests/getdbrec.c \
		-O2 -s -I./core/include -L./core/lib -lkvdb_lib \
		-Wl,-rpath=./bin -o bin/get-db-rec