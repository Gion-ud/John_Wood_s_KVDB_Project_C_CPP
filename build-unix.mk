build-bin: add_bin_to_path bin/libkvdb.so bin/mkdb bin/dbdmp bin/dbget bin/dbput bin/dblskeys bin/dbdel
# mktbldb dmptbldb gettbldbrec
add_bin_to_path:
	echo 'export PATH="$$PATH:./bin"'

CFLAGS = -O2 -fPIC -Wall -Wextra# -Werror
LDFLAGS = -s

MODULE_OBJ = build/kvdb.o build/kvdb_print.o build/hash_func_module.o build/hash_index_lib.o build/global_func.o
CC = gcc


bin/libkvdb.so: $(MODULE_OBJ) | build utils core/src core/include bin
	$(CC) -shared -s $(MODULE_OBJ) -I./core/include -o $@ -fvisibility=hidden

bin/mktbldb: tbldb-utils/mktbldb.c bin/libkvdb.so build/txt_tok_lib.o | core/include core/src bin tests
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o $@
# -g -fsanitize=address

bin/dmptbldb: tbldb-utils/dmptbldb.c bin/libkvdb.so build/txt_tok_lib.o | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o $@

bin/gettbldbrec: tbldb-utils/gettbldbrec.c bin/libkvdb.so | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ \
		-I./core/include -Lbin -lkvdb -Wl,-rpath=./bin -o $@

bin/mkdb: utils/mkdb.c build/txt_tok_lib.o bin/libkvdb.so | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o $@

bin/dbdmp: utils/dbdmp.c bin/libkvdb.so | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o $@

bin/dbget: utils/dbget.c build/txt_tok_lib.o bin/libkvdb.so | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o $@

bin/dbput: utils/dbput.c build/txt_tok_lib.o bin/libkvdb.so | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o $@

bin/dblskeys: utils/dblskeys.c bin/libkvdb.so | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o $@
bin/dbdel: utils/dbdel.c bin/libkvdb.so | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o $@

bin/dbdel-by-id: utils/dbdel-by-id.c bin/libkvdb.so | bin
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o $@
