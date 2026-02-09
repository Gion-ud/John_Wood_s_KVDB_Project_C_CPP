SHELL := /usr/bin/sh

#ifeq ($(OS),Windows_NT)
#$(MAKE) build-win.mk
#else
#$(MAKE) build-unix.mk
#endif

# all: build-bin

build-bin: mkdb_ldll rddb_ldll


build bin lib:
	mkdir -p build bin core/lib

build/db_lib.o: core/src/db_lib.c | build core/src core/include
	gcc -O2 -fPIC -I./core/include -c core/src/db_lib.c -o $@

build/hash_func_module.o: core/src/hash_func_module.c | build core/src core/include
	gcc -O2 -fPIC -I./core/include -c core/src/hash_func_module.c -o $@

build/hash_index_lib.o: core/src/hash_index_lib.c | build core/src core/include
	gcc -O2 -fPIC -I./core/include -c core/src/hash_index_lib.c -o $@

build/txt_tok_lib.o: core/src/txt_tok_lib.c | build core/src core/include
	gcc -O2 -fPIC -I./core/include -c core/src/txt_tok_lib.c -o $@

build/global_func.o: core/src/global_func.c | build core/src core/include
	gcc -O2 -fPIC -I./core/include -c core/src/global_func.c -o $@

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
	gcc tests/mkdb.c build/txt_tok_lib.o build/global_func.o -O2 -s -I./core/include -s -Lbin -lkvdb_lib -Wl,-rpath=./bin -o bin/mkdb
# -g -fsanitize=address

rddb_ldll: tests/rddb.c bin/kvdb_lib.dll build/txt_tok_lib.o build/global_func.o | core/include core/src bin tests
	gcc tests/rddb.c build/txt_tok_lib.o build/global_func.o -O2 -s -I./core/include -s -Lbin -lkvdb_lib -Wl,-rpath=./bin -o bin/rddb


new-db: | bin res database
#	gcc tests/mkdb.c -O2 -s -Llib -lkvdb_lib -o bin/mkdb
	./bin/mkdb res/Pokemon.csv 1 database/table0001.db

read-db: | bin database
#	gcc core/src/read.c -O2 -s -Llib -lkvdb_lib -o bin/read
	./bin/rddb database/table0001.db logs/table0001.db.dump.txt


clean:
	rm build/* bin/* core/lib/* 2>nul

clean-all:
	rm database/* bin/* build/* core/lib/* 2>nul
	rm -rf tmp/* 2>nul

lsdir:
	./scripts/ls_dir.py

