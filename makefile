
#Config
LD_METHOD = dynamic
#EndConfig


SHELL := /usr/bin/sh

ifeq ($(OS),Windows_NT)
	BUILD_DLL_TARGETS = bin/kvdb_lib.dll
	BUILD_LDYN_EXEC_TARGETS = mkdb_ldll read_ldll
else
	BUILD_DLL_TARGETS = bin/libkvdb_lib.so
	BUILD_LDYN_EXEC_TARGETS = mkdb_lso read_lso
endif

BUILD_LSTATIC_EXEC_TARGETS = bin/mkdb bin/read


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
build/db_lib.o: core/src/db_lib.c | build
	gcc -O2 -fPIC -c core/src/db_lib.c -o $@

# HTObject core lib
build/hash_table_lib.o: core/src/hash_table_lib.c | build
	gcc -O2 -fPIC -c core/src/hash_table_lib.c -o $@

build/txt_tok_lib.o: core/src/txt_tok_lib.c | build
	gcc -O2 -fPIC -c core/src/txt_tok_lib.c -o $@

# static lib for kvdb
core/lib/libkvdb_lib.a: build/db_lib.o build/hash_table_lib.o build/txt_tok_lib.o | core/lib
	ar rcs core/lib/libkvdb_lib.a build/db_lib.o build/hash_table_lib.o


# dll for kvdb
bin/kvdb_lib.dll: build/db_lib.o build/hash_table_lib.o build/txt_tok_lib.o core/def/kvdb_lib.def | build core/lib bin
	gcc -O2 -s -shared build/db_lib.o build/hash_table_lib.o build/txt_tok_lib.o core/def/kvdb_lib.def \
		-o bin/kvdb_lib.dll \
		-Wl,--kill-at \
		-Wl,--out-implib,core/lib/libkvdb_lib.dll.a
#		-Wl,--output-def,core/lib/kvdb_lib_exports.def
#	dlltool -D $@ -d core/def/kvdb_lib.def -l core/lib/libkvdb_lib.dll.a

# so for kvdb
bin/libkvdb_lib.so: build/db_lib.o build/hash_table_lib.o build/txt_tok_lib.o | build core/lib bin
	gcc -O2 -s -fPIC -shared -o $@ build/db_lib.o build/hash_table_lib.o build/txt_tok_lib.o


# static linking
bin/mkdb: tests/mkdb.c core/lib/libkvdb_lib.a | core/lib bin tests
	gcc -static tests/mkdb.c -O2 -s -Llib -lkvdb_lib -o $@

bin/read: core/src/read.c core/lib/libkvdb_lib.a | core/lib bin tests
	gcc -static core/src/read.c -O2 -s -Llib -lkvdb_lib -o $@


# dynamic linking implicit
mkdb_ldll: tests/mkdb.c bin/kvdb_lib.dll | core/lib bin tests
	gcc tests/mkdb.c -O2 -s -L./core/lib -lkvdb_lib -o bin/mkdb

read_ldll: tests/read.c bin/kvdb_lib.dll | core/lib bin tests
	gcc tests/read.c -O2 -s -L./core/lib -lkvdb_lib -o bin/read

mkdb_lso: tests/mkdb.c bin/libkvdb_lib.so core/src/txt_tok_lib.c | bin tests
	gcc tests/mkdb.c -O2 core/src/txt_tok_lib.c -s -Lbin -lkvdb_lib -Wl,-rpath=./bin -o bin/mkdb
# -g -fsanitize=address

read_lso: tests/read.c bin/libkvdb_lib.so core/src/txt_tok_lib.c | bin tests
	gcc tests/read.c -O2 core/src/txt_tok_lib.c -s -Lbin -lkvdb_lib -Wl,-rpath=./bin -o bin/read


cpp-main: core/src/main.cpp core/src/kvdb_lib.cpp bin/kvdb_lib.dll
	g++ core/src/main.cpp core/src/kvdb_lib.cpp -O2 -s -Llib -lkvdb_lib -o bin/main && ./bin/main;

cpp-new-db:
	./bin/main;

cpp-main-linux: core/src/main.cpp core/src/kvdb_lib.cpp bin/libkvdb_lib.so
	g++ core/src/main.cpp core/src/kvdb_lib.cpp -O2 -s -Lbin -lkvdb_lib -Wl,-rpath=./bin -o bin/main
	./bin/main


new-db: | bin
#	gcc tests/mkdb.c -O2 -s -Llib -lkvdb_lib -o bin/mkdb
	./bin/mkdb

read-db: | bin
#	gcc core/src/read.c -O2 -s -Llib -lkvdb_lib -o bin/read
	./bin/read


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
