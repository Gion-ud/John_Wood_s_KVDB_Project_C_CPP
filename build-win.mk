build-bin: add_bin_to_path bin/kvdb.dll mkdb_ldll dmpdb_ldll dbget_ldll dblskeys_ldll
add_bin_to_path:
	echo 'set PATH=%CD%\bin;%PATH%'

MODULE_OBJ = build/kvdb.o build/kvdb_print.o build/hash_func_module.o build/hash_index_lib.o build/global_func.o

CFLAGS = -O2 -Wall -Wextra -Werror
LDFLAGS = -s

CC = gcc


# dll for kvdb
bin/kvdb.dll: $(MODULE_OBJ) | bin
	$(CC) -shared -s -DDLL_EXPORT $(MODULE_OBJ) \
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

mkdb_ldll: utils/mkdb.c build/txt_tok_lib.o bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Llib -lkvdb \
		-Wl,-rpath=./bin -o bin/mkdb

dmpdb_ldll: utils/dmpdb.c bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Llib -lkvdb \
		-Wl,-rpath=./bin -o bin/dmpdb

dbget_ldll: utils/dbget.c build/txt_tok_lib.o bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Llib -lkvdb \
		-Wl,-rpath=./bin -o bin/dbget

dblskeys_ldll: utils/dblskeys.c bin/kvdb.dll lib/libkvdb.dll.a | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Llib -lkvdb \
		-Wl,-rpath=./bin -o bin/dblskeys