SHELL := /usr/$(BIN)/sh

CC = gcc

INCLUDE = ../core/include
SRC = .
BUILD = build
CORE = ../core
CORE_BUILD = $(CORE)/build
LIB = $(CORE)/lib
BIN = ../bin

all: add_bin_to_path \
	$(BIN)/newdb $(BIN)/mkdb $(BIN)/dbdmp $(BIN)/dbget \
	$(BIN)/dblskeys $(BIN)/dbput $(BIN)/dbdel $(BIN)/dbdel-by-id \
	$(BIN)/dbresize $(BIN)/dbcompact $(BIN)/dbdmpkv

# mktbldb dmptbldb gettbldbrec
add_bin_to_path:
	echo 'export PATH="$$PATH:$(BIN)"'

LDFLAGS = -s

CC = gcc

$(BIN)/newdb: $(BUILD)/kvdb/newdb.o $(BIN)/kvdb.dll $(LIB)/libkvdb.dll.a | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(LIB) -lkvdb -o $@

$(BIN)/mkdb: $(BUILD)/kvdb/mkdb.o $(CORE_BUILD)/strutils/txt_tok.o $(BIN)/kvdb.dll $(LIB)/libkvdb.dll.a | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(LIB) -lkvdb -o $@

$(BIN)/dbdmp: $(BUILD)/kvdb/dbdmp.o $(BIN)/kvdb.dll $(LIB)/libkvdb.dll.a | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(LIB) -lkvdb -o $@

$(BIN)/dbget: $(BUILD)/kvdb/dbget.o $(CORE_BUILD)/strutils/txt_tok.o $(BIN)/kvdb.dll $(LIB)/libkvdb.dll.a | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(LIB) -lkvdb -o $@

$(BIN)/dbput: $(BUILD)/kvdb/dbput.o $(CORE_BUILD)/strutils/txt_tok.o $(BIN)/kvdb.dll $(LIB)/libkvdb.dll.a | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(LIB) -lkvdb -o $@

$(BIN)/dbdel: $(BUILD)/kvdb/dbdel.o $(BIN)/kvdb.dll $(LIB)/libkvdb.dll.a | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(LIB) -lkvdb -o $@

$(BIN)/dblskeys: $(BUILD)/kvdb/dblskeys.o $(BIN)/kvdb.dll $(LIB)/libkvdb.dll.a | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(LIB) -lkvdb -o $@

$(BIN)/dbresize: $(BUILD)/kvdb/dbresize.o $(BIN)/kvdb.dll $(LIB)/libkvdb.dll.a | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(LIB) -lkvdb -o $@

$(BIN)/dbcompact: $(BUILD)/kvdb/dbcompact.o $(BIN)/kvdb.dll $(LIB)/libkvdb.dll.a | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(LIB) -lkvdb -o $@

$(BIN)/dbdmpkv: $(BUILD)/kvdb/dbdmpkv.o $(BIN)/kvdb.dll $(LIB)/libkvdb.dll.a | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(LIB) -lkvdb -o $@

$(BIN)/dbdel-by-id: $(BUILD)/kvdb/dbdel-by-id.o $(BIN)/kvdb.dll $(LIB)/libkvdb.dll.a | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(LIB) -lkvdb -o $@

