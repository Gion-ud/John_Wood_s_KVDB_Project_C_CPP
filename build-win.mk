build-bin: add_bin_to_path bin/kvdb.dll bin/mkdb bin/dbdmp bin/dbget bin/dblskeys bin/dbput bin/dbdel bin/dbdel-by-id
add_bin_to_path:
	echo 'set PATH=%CD%\bin;%PATH%' # windows
	echo 'export PATH="$$PATH:./bin"' # unix

MODULE_OBJ = build/kvdb.o build/kvdb_print.o build/hash_func_module.o build/hash_index_lib.o build/global_func.o

CFLAGS = -O2 -Wall -Wextra -Werror
LDFLAGS = 
CC = cc


# dll for kvdb
bin/kvdb.dll: $(MODULE_OBJ) | bin
	$(CC) -shared -s $(MODULE_OBJ) \
		-I./core/include -o $@ \
		-Wl,--kill-at \
		-Wl,--out-implib,lib/libkvdb.dll.a \
		-Wl,--output-def,lib/kvdb_dll.def

mktbldb_ldll: utils/mktbldb.c bin/kvdb.dll lib/libkvdb.dll.a build/txt_tok_lib.o | core/include core/src bin tests
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Llib -lkvdb \
		-Wl,-rpath=./bin -o bin/tbldb-new

# -g -fsanitize=address

dmptbldb_ldll: utils/dmptbldb.c bin/kvdb.dll lib/libkvdb.dll.a build/txt_tok_lib.o | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Llib -lkvdb \
		-Wl,-rpath=./bin -o bin/tbldb-dump

gettbldbrec_ldll: utils/gettbldbrec.c bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ \
		-I./core/include -Llib -lkvdb -Wl,-rpath=./bin -o bin/tbldb-get

bin/mkdb: utils/mkdb.c build/txt_tok_lib.o bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Llib -lkvdb \
		-Wl,-rpath=./bin -o $@

bin/dbdmp: utils/dbdmp.c bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Llib -lkvdb \
		-Wl,-rpath=./bin -o $@

bin/dbget: utils/dbget.c build/txt_tok_lib.o bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Llib -lkvdb \
		-Wl,-rpath=./bin -o $@

bin/dbput: utils/dbput.c build/txt_tok_lib.o bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Llib -lkvdb \
		-Wl,-rpath=./bin -o $@

bin/dblskeys: utils/dblskeys.c bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Llib -lkvdb \
		-Wl,-rpath=./bin -o $@

bin/dbdel: utils/dbdel.c bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Llib -lkvdb \
		-Wl,-rpath=./bin -o $@

bin/dbdel-by-id: utils/dbdel-by-id.c bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Llib -lkvdb \
		-Wl,-rpath=./bin -o $@

