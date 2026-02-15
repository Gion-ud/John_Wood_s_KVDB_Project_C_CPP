build-bin: add_bin_to_path bin/libkvdb.so mkdb_lso dmpdb_lso dbget_lso dblskeys_lso
# mktbldb_lso dmptbldb_lso gettbldbrec_lso
add_bin_to_path:
	echo 'export PATH="$$PATH:./bin"'

CFLAGS = -O2 -fPIC -Wall -Wextra -Werror
LDFLAGS = -s

MODULE_OBJ = build/kvdb.o build/kvdb_print.o build/hash_func_module.o build/hash_index_lib.o build/global_func.o
CC = gcc


bin/libkvdb.so: $(MODULE_OBJ) | build utils core/src core/include bin
	$(CC) -shared -s $(MODULE_OBJ) -I./core/include -o $@ -fvisibility=hidden

mktbldb_lso: tbldb-utils/mktbldb.c bin/libkvdb.so build/txt_tok_lib.o | core/include core/src bin tests
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o bin/tbldb-new
# -g -fsanitize=address

dmptbldb_lso: tbldb-utils/dmptbldb.c bin/libkvdb.so build/txt_tok_lib.o | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o bin/tbldb-dump

gettbldbrec_lso: tbldb-utils/gettbldbrec.c bin/libkvdb.so | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ \
		-I./core/include -Lbin -lkvdb -Wl,-rpath=./bin -o bin/tbldb-get

mkdb_lso: utils/mkdb.c build/txt_tok_lib.o bin/libkvdb.so | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o bin/mkdb

dmpdb_lso: utils/dmpdb.c bin/libkvdb.so | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o bin/dmpdb

dbget_lso: utils/dbget.c build/txt_tok_lib.o bin/libkvdb.so | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o bin/dbget

dblskeys_lso: utils/dblskeys.c bin/libkvdb.so | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o bin/dblskeys