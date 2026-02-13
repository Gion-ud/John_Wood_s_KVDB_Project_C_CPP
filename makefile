SHELL := /usr/bin/sh

CFLAGS = -O2 -fPIC -Wall -Wextra -Werror

MODULE_OBJ = build/kvdb.o build/kvdb_print.o build/hash_func_module.o build/hash_index_lib.o build/global_func.o

all: $(MODULE_OBJ) build/txt_tok_lib.o build-bin

build bin lib database:
	mkdir -p build bin core/lib database

build/kvdb.o: core/src/kvdb.c | build
	gcc $(CFLAGS) -DDLL_EXPORT -I./core/include -c $^ -o $@

build/kvdb_print.o: core/src/kvdb_print.c | build
	gcc $(CFLAGS) -DDLL_EXPORT -I./core/include -c $^ -o $@

build/hash_func_module.o: core/src/hash_func_module.c | build
	gcc $(CFLAGS) -I./core/include -c $^ -o $@

build/hash_index_lib.o: core/src/hash_index_lib.c | build
	gcc $(CFLAGS) -I./core/include -c $^ -o $@

build/txt_tok_lib.o: core/src/txt_tok_lib.c | build
	gcc $(CFLAGS) -I./core/include -c $^ -o $@

build/global_func.o: core/src/global_func.c | build
	gcc $(CFLAGS) -I./core/include -c $^ -o $@

build-bin:
ifeq ($(OS),Windows_NT)
	$(MAKE) -f build-win.mk build-bin
else
	$(MAKE) -f build-unix.mk build-bin
endif


new-db: | bin res database
	./bin/mkdb res/Pokemon.csv 1 database/table0001.db

read-db: | bin database
	./bin/rddb database/table0001.db logs/table0001.db.dump.txt

get-db-rec: | bin database
	./bin/get-db-rec

clean:
	rm build/* bin/* core/lib/* 2>nul

clean-all:
	rm database/* bin/* build/* core/lib/* logs/* 2>nul
	rm -rf tmp/* 2>nul

lsdir:
	./scripts/ls_dir.py

