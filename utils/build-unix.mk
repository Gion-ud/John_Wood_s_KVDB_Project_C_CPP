SHELL := /usr/$(BIN)/sh

CC = gcc

INCLUDE = ../core/include
SRC = .
BUILD = build
CORE = ../core
CORE_BUILD = $(CORE)/build
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

$(BIN)/mkdb: $(BUILD)/kvdb/mkdb.o $(CORE_BUILD)/strutils/txt_tok.o $(BIN)/libkvdb.so | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(BIN) -lkvdb -Wl,-rpath=$(BIN) -o $@

$(BIN)/newdb: $(BUILD)/kvdb/newdb.o $(BIN)/libkvdb.so | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(BIN) -lkvdb -Wl,-rpath=$(BIN) -o $@

$(BIN)/dbdmp: $(BUILD)/kvdb/dbdmp.o $(BIN)/libkvdb.so | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(BIN) -lkvdb -Wl,-rpath=$(BIN) -o $@

$(BIN)/dbget: $(BUILD)/kvdb/dbget.o $(CORE_BUILD)/strutils/txt_tok.o $(BIN)/libkvdb.so | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(BIN) -lkvdb -Wl,-rpath=$(BIN) -o $@

$(BIN)/dbput: $(BUILD)/kvdb/dbput.o $(CORE_BUILD)/strutils/txt_tok.o $(BIN)/libkvdb.so | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(BIN) -lkvdb -Wl,-rpath=$(BIN) -o $@

$(BIN)/dblskeys: $(BUILD)/kvdb/dblskeys.o $(BIN)/libkvdb.so | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(BIN) -lkvdb -Wl,-rpath=$(BIN) -o $@

$(BIN)/dbdel: $(BUILD)/kvdb/dbdel.o $(BIN)/libkvdb.so | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(BIN) -lkvdb -Wl,-rpath=$(BIN) -o $@

$(BIN)/dbresize: $(BUILD)/kvdb/dbresize.o $(BIN)/kvdb.dll $(BIN)/libkvdb.so | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(BIN) -lkvdb -Wl,-rpath=$(BIN) -o $@

$(BIN)/dbcompact: $(BUILD)/kvdb/dbcompact.o $(BIN)/kvdb.dll $(BIN)/libkvdb.so | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(BIN) -lkvdb -Wl,-rpath=$(BIN) -o $@

$(BIN)/dbdmpkv: $(BUILD)/kvdb/dbdmpkv.o $(BIN)/kvdb.dll $(BIN)/libkvdb.so | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(BIN) -lkvdb -Wl,-rpath=$(BIN) -o $@

$(BIN)/dbdel-by-id: $(BUILD)/kvdb/dbdel-by-id.c $(BIN)/libkvdb.so | $(BIN)
	$(CC) $(LDFLAGS) $^ -L$(BIN) -lkvdb -Wl,-rpath=$(BIN) -o $@
