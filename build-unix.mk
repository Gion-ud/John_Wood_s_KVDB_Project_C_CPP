all: add_bin_to_path bin/libkvdb.so bin/mkdb bin/newdb bin/dbdmp bin/dbget bin/dbput bin/dblskeys bin/dbdel
# mktbldb dmptbldb gettbldbrec
add_bin_to_path:
	echo 'export PATH="$$PATH:./bin"'

LDFLAGS = -s

MODULE_OBJ = build/kvdb.o build/kvdb_print.o build/hash_func_module.o build/hash_index.o build/global_utils.o
CC = gcc


bin/libkvdb.so: $(MODULE_OBJ) | build utils core/src core/include bin
	$(CC) -shared -s $(MODULE_OBJ) -o $@ -fvisibility=hidden

bin/mkdb: build/mkdb.o build/txt_tok.o bin/libkvdb.so | bin
	$(CC) $(LDFLAGS) $^ -Lbin -lkvdb -Wl,-rpath=./bin -o $@

bin/newdb: build/newdb.o bin/libkvdb.so | bin
	$(CC) $(LDFLAGS) $^ -Lbin -lkvdb -Wl,-rpath=./bin -o $@

bin/dbdmp: build/dbdmp.o bin/libkvdb.so | bin
	$(CC) $(LDFLAGS) $^ -Lbin -lkvdb -Wl,-rpath=./bin -o $@

bin/dbget: build/dbget.o build/txt_tok.o bin/libkvdb.so | bin
	$(CC) $(LDFLAGS) $^ -Lbin -lkvdb -Wl,-rpath=./bin -o $@

bin/dbput: build/dbput.o build/txt_tok.o bin/libkvdb.so | bin
	$(CC) $(LDFLAGS) $^ -Lbin -lkvdb -Wl,-rpath=./bin -o $@

bin/dblskeys: build/dblskeys.o bin/libkvdb.so | bin
	$(CC) $(LDFLAGS) $^ -Lbin -lkvdb -Wl,-rpath=./bin -o $@

bin/dbdel: build/dbdel.o bin/libkvdb.so | bin
	$(CC) $(LDFLAGS) $^ -Lbin -lkvdb -Wl,-rpath=./bin -o $@

bin/dbresize: build/dbresize.o bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(LDFLAGS) $^ -Llib -lkvdb -Wl,-rpath=./bin -o $@

bin/dbcompact: build/dbcompact.o bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(LDFLAGS) $^ -Llib -lkvdb -Wl,-rpath=./bin -o $@

bin/dbdmpkv: build/dbdmpkv.o bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(LDFLAGS) $^ -Llib -lkvdb -Wl,-rpath=./bin -o $@

bin/dbdel-by-id: build/dbdel-by-id.c bin/libkvdb.so | bin
	$(CC) $(LDFLAGS) $^ -Lbin -lkvdb -Wl,-rpath=./bin -o $@
