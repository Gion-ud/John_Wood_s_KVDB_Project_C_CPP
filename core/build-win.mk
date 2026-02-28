BUILD = build
LIB = lib
BIN = ../bin

all: add_bin_to_path $(BIN)/kvdb.dll

add_bin_to_path:
	# export PATH="$$PATH:$(BIN)"	## unix

MODULE_OBJ = $(BUILD)/kvdb/kvdb.o $(BUILD)/kvdb/kvdb_print.o \
             $(BUILD)/hash/hash_func_module.o \
             $(BUILD)/hash/hash_index_lib.o $(BUILD)/global_utils.o

CC = cc


$(BIN)/kvdb.dll: $(MODULE_OBJ) | $(BIN)
	$(CC) -shared $(MODULE_OBJ) \
		-o $@ \
		-Wl,--kill-at \
		-Wl,--out-implib,$(LIB)/libkvdb.dll.a \
		-Wl,--output-def,$(LIB)/kvdb_dll.def

#		-Wl,--exclude-all-symbols \
