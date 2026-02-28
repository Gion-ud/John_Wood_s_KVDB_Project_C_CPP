BUILD = build
LIB = lib
BIN = ../bin

CC = gcc
LDFLAGS = -s

all: add_bin_to_path $(BIN)/libkvdb.so

add_bin_to_path:
	echo 'export PATH="$$PATH:$(BIN)"'


MODULE_OBJ = $(BUILD)/kvdb/kvdb.o $(BUILD)/kvdb/kvdb_print.o \
             $(BUILD)/hash/hash_func_module.o \
             $(BUILD)/hash/hash_index_lib.o $(BUILD)/global_utils.o

$(BIN)/libkvdb.so: $(MODULE_OBJ) | $(BIN)
	$(CC) -shared $(LDFLAGS) $(MODULE_OBJ) -o $@