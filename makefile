SHELL := /usr/bin/sh


ifeq ($(OS),Windows_NT)
CFLAGS = -O2 -Wextra -Werror
else
CFLAGS = -O2 -fPIC -Wall -Wextra -Werror
endif

MODULE_OBJ = build/kvdb.o build/kvdb_print.o build/hash_func_module.o \
             build/hash_index_lib.o build/global_utils.o build/newdb.o \
             build/mkdb.o build/dbdmp.o build/dbget.o build/dbput.o \
             build/dbdel.o build/dblskeys.o build/dbresize.o \
             build/dbcompact.o build/dbdmpkv.o build/dbdel-by-id.o

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

build/global_utils.o: core/src/global_utils.c | build
	$(CC) $(CFLAGS) -I./core/include -c $< -o $@


build/newdb.o: utils/newdb.c | build
	$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -I./core/include -o $@

build/mkdb.o: utils/mkdb.c build/txt_tok_lib.o | build
	$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -I./core/include -o $@

build/dbdmp.o: utils/dbdmp.c | build
	$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -I./core/include -o $@

build/dbget.o: utils/dbget.c | build
	$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -I./core/include -o $@

build/dbput.o: utils/dbput.c | build
	$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -I./core/include -o $@

build/dbdel.o: utils/dbdel.c | build
	$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -I./core/include -o $@

build/dblskeys.o: utils/dblskeys.c | build
	$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -I./core/include -o $@

build/dbresize.o: utils/dbresize.c | build
	$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -I./core/include -o $@

build/dbcompact.o: utils/dbcompact.c | build
	$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -I./core/include -o $@

build/dbdmpkv.o: utils/dbdmpkv.c | build
	$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -I./core/include -o $@

build/dbdel-by-id.o: utils/dbdel-by-id.c | build
	$(CC) $(CFLAGS) $(LDFLAGS) -c $^ -I./core/include -o $@




.PHONY: build-bin
build-bin:
ifeq ($(OS),Windows_NT)
	$(MAKE) CC=$(CC) -f build-win.mk
else
	$(MAKE) CC=$(CC) -f build-unix.mk
endif

clean:
	rm build/* bin/* core/lib/*

clean-all:
	rm database/* bin/* build/* lib/* logs/* *.db && rm -rf tmp/*

lsdir:
	./scripts/ls_dir.py



