SHELL := /usr/bin/sh


ifeq ($(OS),Windows_NT)
CFLAGS = -O2 -Wextra -Werror
else
CFLAGS = -O2 -fPIC -Wall -Wextra# -Werror
endif

MODULE_OBJ = build/kvdb.o build/kvdb_print.o build/hash_func_module.o build/hash_index_lib.o build/global_func.o
CC = gcc

all: $(MODULE_OBJ) build/txt_tok_lib.o build-bin

build bin lib database:
	mkdir -p build bin core/lib database

build/kvdb.o: core/src/kvdb.c | build
	$(CC) $(CFLAGS) -DDLL_EXPORT -I./core/include -c $< -o $@

build/kvdb_print.o: core/src/kvdb_print.c | build
	$(CC) $(CFLAGS) -DDLL_EXPORT -I./core/include -c $< -o $@

build/hash_func_module.o: core/src/hash_func_module.c | build
	$(CC) $(CFLAGS) -I./core/include -c $< -o $@

build/hash_index_lib.o: core/src/hash_index_lib.c | build
	$(CC) $(CFLAGS) -I./core/include -c $< -o $@

build/txt_tok_lib.o: core/src/txt_tok_lib.c | build
	$(CC) $(CFLAGS) -I./core/include -c $< -o $@

build/global_func.o: core/src/global_func.c | build
	$(CC) $(CFLAGS) -I./core/include -c $< -o $@

build-bin:
ifeq ($(OS),Windows_NT)
	$(MAKE) CC=$(CC) -f build-win.mk build-bin
else
	$(MAKE) CC=$(CC) -f build-unix.mk build-bin
endif

clean:
	rm build/* bin/* core/lib/*

clean-all:
	rm database/* bin/* build/* lib/* logs/* *.db && rm -rf tmp/*

lsdir:
	./scripts/ls_dir.py

