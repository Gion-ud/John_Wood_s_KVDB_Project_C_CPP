SHELL := /usr/bin/sh


ifeq ($(OS),Windows_NT)
CFLAGS = -O2 -Wextra -Werror
else
CFLAGS = -O2 -fPIC -Wall -Wextra -Werror
endif

CC = gcc

INCLUDE = ../core/include
SRC = .
BUILD = build

all: \
	$(BUILD)/kvdb \
	$(BUILD)/kvdb/newdb.o $(BUILD)/kvdb/mkdb.o $(BUILD)/kvdb/dbdmp.o \
	$(BUILD)/kvdb/dbget.o $(BUILD)/kvdb/dbput.o $(BUILD)/kvdb/dbdel.o \
	$(BUILD)/kvdb/dblskeys.o $(BUILD)/kvdb/dbresize.o $(BUILD)/kvdb/dbcompact.o \
	$(BUILD)/kvdb/dbdmpkv.o $(BUILD)/kvdb/dbdel-by-id.o \
	build-dll build-bin

$(BUILD)/kvdb:
	mkdir -p $@

$(BUILD)/kvdb/newdb.o: $(SRC)/kvdb/newdb.c | $(BUILD)
	$(CC) $(CFLAGS) -c $^ -I$(INCLUDE) -o $@

$(BUILD)/kvdb/mkdb.o: $(SRC)/kvdb/mkdb.c | $(BUILD)
	$(CC) $(CFLAGS) -c $^ -I$(INCLUDE) -o $@

$(BUILD)/kvdb/dbdmp.o: $(SRC)/kvdb/dbdmp.c | $(BUILD)
	$(CC) $(CFLAGS) -c $^ -I$(INCLUDE) -o $@

$(BUILD)/kvdb/dbget.o: $(SRC)/kvdb/dbget.c | $(BUILD)
	$(CC) $(CFLAGS) -c $^ -I$(INCLUDE) -o $@

$(BUILD)/kvdb/dbput.o: $(SRC)/kvdb/dbput.c | $(BUILD)
	$(CC) $(CFLAGS) -c $^ -I$(INCLUDE) -o $@

$(BUILD)/kvdb/dbdel.o: $(SRC)/kvdb/dbdel.c | $(BUILD)
	$(CC) $(CFLAGS) -c $^ -I$(INCLUDE) -o $@

$(BUILD)/kvdb/dblskeys.o: $(SRC)/kvdb/dblskeys.c | $(BUILD)
	$(CC) $(CFLAGS) -c $^ -I$(INCLUDE) -o $@

$(BUILD)/kvdb/dbresize.o: $(SRC)/kvdb/dbresize.c | $(BUILD)
	$(CC) $(CFLAGS) -c $^ -I$(INCLUDE) -o $@

$(BUILD)/kvdb/dbcompact.o: $(SRC)/kvdb/dbcompact.c | $(BUILD)
	$(CC) $(CFLAGS) -c $^ -I$(INCLUDE) -o $@

$(BUILD)/kvdb/dbdmpkv.o: $(SRC)/kvdb/dbdmpkv.c | $(BUILD)
	$(CC) $(CFLAGS) -c $^ -I$(INCLUDE) -o $@

$(BUILD)/kvdb/dbdel-by-id.o: $(SRC)/kvdb/dbdel-by-id.c | $(BUILD)
	$(CC) $(CFLAGS) -c $^ -I$(INCLUDE) -o $@


build-dll:
	cd ../core && make

.PHONY: build-bin
build-bin:
ifeq ($(OS),Windows_NT)
	$(MAKE) CC=$(CC) -f build-win.mk
else
	$(MAKE) CC=$(CC) -f build-unix.mk
endif
clean:
	rm -rf $(BUILD)/*


