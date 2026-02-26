all: add_bin_to_path bin/kvdb.dll bin/newdb bin/mkdb \
     bin/dbdmp bin/dbget bin/dblskeys bin/dbput \
     bin/dbdel bin/dbdel-by-id bin/dbresize bin/dbcompact \
     bin/dbdmpkv

add_bin_to_path:
	# set PATH=%CD%\bin;%PATH% 	## windows
	# export PATH="$$PATH:./bin" 	## unix

MODULE_OBJ = build/kvdb.o build/kvdb_print.o build/hash_func_module.o build/hash_index_lib.o build/global_utils.o

LDFLAGS = 
CC = cc


# dll for kvdb
bin/kvdb.dll: $(MODULE_OBJ) | bin
	$(CC) -shared -s $(MODULE_OBJ) \
		-o $@ \
		-Wl,--kill-at \
		-Wl,--out-implib,lib/libkvdb.dll.a \
		-Wl,--output-def,lib/kvdb_dll.def

bin/newdb: build/newdb.o bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -Llib -lkvdb -o $@

bin/mkdb: build/mkdb.o build/txt_tok_lib.o bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -Llib -lkvdb -o $@

bin/dbdmp: build/dbdmp.o bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -Llib -lkvdb -o $@

bin/dbget: build/dbget.o build/txt_tok_lib.o bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -Llib -lkvdb -o $@

bin/dbput: build/dbput.o build/txt_tok_lib.o bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -Llib -lkvdb -o $@

bin/dbdel: build/dbdel.o bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -Llib -lkvdb -o $@

bin/dblskeys: build/dblskeys.o bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -Llib -lkvdb -o $@

bin/dbresize: build/dbresize.o bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -Llib -lkvdb -o $@

bin/dbcompact: build/dbcompact.o bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -Llib -lkvdb -o $@

bin/dbdmpkv: build/dbdmpkv.o bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -Llib -lkvdb -o $@

bin/dbdel-by-id: build/dbdel-by-id.o bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -Llib -lkvdb -o $@

