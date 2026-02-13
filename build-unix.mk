build-bin: add_bin_to_path bin/libkvdb.so mkdb_lso dmpdb_lso dbget_lso dblskeys_lso
# mktbldb_lso dmptbldb_lso gettbldbrec_lso
add_bin_to_path:
	echo 'export PATH="$$PATH:./bin"'


CFLAGS = -O2 -s -Wall -Wextra -Werror

MODULE_OBJ = build/kvdb.o build/kvdb_print.o build/hash_func_module.o build/hash_index_lib.o build/global_func.o

bin/libkvdb.so: $(MODULE_OBJ) | build utils core/src core/include bin
	gcc $(CFLAGS) -shared $(MODULE_OBJ) -I./core/include -o $@ -fvisibility=hidden

mktbldb_lso: utils/mktbldb.c bin/libkvdb.so build/txt_tok_lib.o | core/include core/src bin tests
	gcc $(CFLAGS) -fPIC $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o bin/tbldb-new
# -g -fsanitize=address

dmptbldb_lso: utils/dmptbldb.c bin/libkvdb.so build/txt_tok_lib.o | bin
	gcc $(CFLAGS) -fPIC $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o bin/tbldb-dump

gettbldbrec_lso: utils/gettbldbrec.c bin/libkvdb.so | bin
	gcc $(CFLAGS) -fPIC $^ \
		-I./core/include -Lbin -lkvdb -Wl,-rpath=./bin -o bin/tbldb-get

mkdb_lso: utils/mkdb.c build/txt_tok_lib.o bin/libkvdb.so | bin
	gcc $(CFLAGS) -fPIC $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o bin/mkdb

dmpdb_lso: utils/dmpdb.c bin/libkvdb.so | bin
	gcc $(CFLAGS) -fPIC $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o bin/dmpdb

dbget_lso: utils/dbget.c build/txt_tok_lib.o bin/libkvdb.so | bin
	gcc $(CFLAGS) -fPIC $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o bin/dbget

dblskeys_lso: utils/dblskeys.c bin/libkvdb.so | bin
	gcc $(CFLAGS) -fPIC $^ -I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o bin/dblskeys