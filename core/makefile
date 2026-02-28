SHELL := /usr/bin/sh

CC = cc
SRC = src
INCLUDE = include

BUILD = build
LIB = lib
BIN = ../bin


ifeq ($(OS),Windows_NT)
CFLAGS = -O2 -Wextra -Werror -fvisibility=hidden
else
CFLAGS = -O2 -fPIC -Wall -Wextra -Werror -fvisibility=hidden
endif

MODULE_OBJ = \
	$(BUILD)/kvdb/kvdb.o $(BUILD)/kvdb/kvdb_print.o \
	$(BUILD)/hash/hash_func_module.o $(BUILD)/hash/hash_index_lib.o \
	$(BUILD)/global_utils.o

all: output-dirs $(MODULE_OBJ) \
	$(BUILD)/strutils/txt_tok.o build-dll

output-dirs:
	mkdir -p $(BUILD) $(BIN) $(LIB)
	mkdir -p $(BUILD)/kvdb
	mkdir -p $(BUILD)/hash
	mkdir -p $(BUILD)/strutils

$(BUILD)/kvdb/kvdb.o: $(SRC)/kvdb/kvdb.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@

$(BUILD)/kvdb/kvdb_print.o: $(SRC)/kvdb/kvdb_print.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@

$(BUILD)/hash/hash_func_module.o: $(SRC)/hash/hash_func_module.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@

$(BUILD)/hash/hash_index_lib.o: $(SRC)/hash/hash_index.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@

$(BUILD)/strutils/txt_tok.o: $(SRC)/strutils/txt_tok.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@

$(BUILD)/global_utils.o: $(SRC)/global_utils.c | $(BUILD)
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@


.PHONY: build-dll
build-dll:
ifeq ($(OS),Windows_NT)
	$(MAKE) CC=$(CC) -f build-win.mk
else
	$(MAKE) CC=$(CC) -f build-unix.mk
endif

clean:
	rm -rf $(BUILD)/*



