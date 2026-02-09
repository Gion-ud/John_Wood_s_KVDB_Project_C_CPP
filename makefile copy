
#Config
LD_METHOD = dynamic
#EndConfig


SHELL := /usr/bin/sh

ifeq ($(OS),Windows_NT)
	BUILD_DLL_TARGETS = bin/kvdb_lib.dll
	BUILD_LDYN_EXEC_TARGETS = mkdb_ldll rddb_ldll getdbrec_ldll
else
	BUILD_DLL_TARGETS = bin/libkvdb_lib.so
	BUILD_LDYN_EXEC_TARGETS = mkdb_lso rddb_lso
endif

BUILD_LSTATIC_EXEC_TARGETS = mkdb_lstatic rddb_lstatic


ifeq ($(LD_METHOD),static)
	BUILD_LIB_TARGETS = core/lib/libkvdb_lib.a
	BUILD_EXEC_TARGETS = $(BUILD_LSTATIC_EXEC_TARGETS)
else
	BUILD_LIB_TARGETS = $(BUILD_DLL_TARGETS)
	BUILD_EXEC_TARGETS = $(BUILD_LDYN_EXEC_TARGETS)
endif


all: 

build-static-lib: core/lib/libkvdb_lib.a
build-lstatic-exec: $(BUILD_LSTATIC_EXEC_TARGETS)

build-dyn-lib: $(BUILD_DLL_TARGETS)
build-dynld-exec: $(BUILD_LDYN_EXEC_TARGETS)

build-lib: $(BUILD_LIB_TARGETS)
build-exec: $(BUILD_EXEC_TARGETS)
build-bin: $(BUILD_EXEC_TARGETS)




build bin lib:
	mkdir -p build bin core/lib
#	mkdir -p build bin lib

# kvdb core lib
build/db_lib.o: core/src/db_lib.c | build core/src core/include
	gcc -O2 -fPIC -I./core/include -c core/src/db_lib.c -o $@

# HTObject core lib
build/hash_table_lib.o: core/src/hash_table_lib.c | build core/src core/include
	gcc -O2 -fPIC -I./core/include -c core/src/hash_table_lib.c -o $@

# for parsing and tokenising text
build/txt_tok_lib.o: core/src/txt_tok_lib.c | build core/src core/include
	gcc -O2 -fPIC -I./core/include -c core/src/txt_tok_lib.c -o $@

# global utils
build/global_func.o: core/src/global_func.c | build core/src core/include
	gcc -O2 -fPIC -I./core/include -c core/src/global_func.c -o $@

MODULE_OBJ = build/db_lib.o build/hash_table_lib.o build/txt_tok_lib.o build/global_func.o

# static lib for kvdb
core/lib/libkvdb_lib.a: build/db_lib.o build/hash_table_lib.o build/txt_tok_lib.o build/global_func.o | core/lib
	ar rcs core/lib/libkvdb_lib.a $(MODULE_OBJ)


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

# so for kvdb
bin/libkvdb_lib.so: $(MODULE_OBJ) | build core/src core/include bin
	gcc -O2 -s -fPIC -shared -I./core/include $(MODULE_OBJ) -o $@


# static linking
mkdb_lstatic: tests/mkdb.c core/lib/libkvdb_lib.a | core/lib core/include bin tests
	gcc -static -I./core/include tests/mkdb.c -O2 -s -L./core/lib  -lkvdb_lib -o bin/mkdb

rddb_lstatic: tests/rddb.c core/lib/libkvdb_lib.a | core/lib core/include bin tests
	gcc -static -I./core/include tests/rddb.c -O2 -s -L./core/lib  -lkvdb_lib -o bin/rddb


# dynamic linking implicit
mkdb_ldll: tests/mkdb.c bin/kvdb_lib.dll | core/include core/src core/lib bin tests
	gcc tests/mkdb.c -O2 -s -I./core/include -L./core/lib -lkvdb_lib -o bin/mkdb

rddb_ldll: tests/rddb.c bin/kvdb_lib.dll | core/include core/src core/lib bin tests
	gcc tests/rddb.c -O2 -s -I./core/include -L./core/lib -lkvdb_lib -o bin/rddb

getdbrec_ldll: tests/getdbrec.c bin/kvdb_lib.dll | core/include core/src core/lib bin tests
	gcc tests/getdbrec.c -O2 -s -I./core/include -L./core/lib -lkvdb_lib -o bin/get-db-rec


mkdb_lso: tests/mkdb.c bin/libkvdb_lib.so core/src/txt_tok_lib.c | core/include core/src bin tests
	gcc tests/mkdb.c -O2 -I./core/include core/src/txt_tok_lib.c -s -Lbin -lkvdb_lib -Wl,-rpath=./bin -o bin/mkdb
# -g -fsanitize=address

rddb_lso: tests/rddb.c bin/libkvdb_lib.so core/src/txt_tok_lib.c | core/include core/src bin tests
	gcc tests/rddb.c -O2 -I./core/include core/src/txt_tok_lib.c -s -Lbin -lkvdb_lib -Wl,-rpath=./bin -o bin/rddb


cpp-main: core/src/main.cpp core/src/kvdb_lib.cpp bin/kvdb_lib.dll
	g++ core/src/main.cpp core/src/kvdb_lib.cpp -O2 -s -Llib -lkvdb_lib -o bin/main && ./bin/main;

cpp-new-db:
	./bin/main;

cpp-main-linux: core/src/main.cpp core/src/kvdb_lib.cpp bin/libkvdb_lib.so
	g++ core/src/main.cpp core/src/kvdb_lib.cpp -O2 -s -Lbin -lkvdb_lib -Wl,-rpath=./bin -o bin/main
	./bin/main


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

dmpbin:
	./scripts/dmpbinhex.sh

gen-thumbs:
	./scripts/gen_thumbnail.py

lsdir:
	./scripts/ls_dir.py

# Windows only!
mkln:
	.\scripts\mkresln.bat

rmln:
	.\scripts\rmresln.bat
