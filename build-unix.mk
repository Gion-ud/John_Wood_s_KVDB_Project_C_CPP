build-bin: add_bin_to_path bin/libkvdb.so mkdb_lso dmpdb_lso dbget_lso
# mktbldb_lso dmptbldb_lso gettbldbrec_lso
add_bin_to_path:
	echo 'export PATH="$$PATH:./bin"'


WARNING_FLAGS = -Wall -Wextra# -Werror

MODULE_OBJ = build/db_lib.o build/hash_func_module.o build/hash_index_lib.o build/global_func.o

# dll for kvdb
bin/libkvdb.so: $(MODULE_OBJ) | build core/src core/include bin
	gcc $(WARNING_FLAGS) -O2 -s -shared $(MODULE_OBJ) -I./core/include -o bin/libkvdb.so


mktbldb_lso: tests/mktbldb.c bin/libkvdb.so build/txt_tok_lib.o build/global_func.o | core/include core/src bin tests
	gcc $(WARNING_FLAGS) -fPIC -O2 -s \
		tests/mktbldb.c build/txt_tok_lib.o build/global_func.o \
		-I./core/include -Lbin -lkvdb -Wl,-rpath=./bin -o bin/tbldb-new

# -g -fsanitize=address

dmptbldb_lso: tests/dmptbldb.c bin/libkvdb.so build/txt_tok_lib.o build/global_func.o | core/include core/src bin tests
	gcc $(WARNING_FLAGS) -fPIC -O2 -s \
		tests/dmptbldb.c build/txt_tok_lib.o build/global_func.o \
		-I./core/include -Lbin -lkvdb -Wl,-rpath=./bin -o bin/tbldb-dump

gettbldbrec_lso: tests/gettbldbrec.c bin/libkvdb.so | core/include core/src core/lib bin tests
	gcc $(WARNING_FLAGS) -fPIC -O2 -s tests/gettbldbrec.c \
		-I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o bin/tbldb-get

mkdb_lso: tests/mkdb.c bin/libkvdb.so | core/include core/src core/lib bin tests
	gcc $(WARNING_FLAGS) -fPIC -O2 -s tests/mkdb.c core/src/txt_tok_lib.c \
		-I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o bin/mkdb

dmpdb_lso: tests/dmpdb.c bin/libkvdb.so | core/include core/src core/lib bin tests
	gcc $(WARNING_FLAGS) -fPIC -O2 -s tests/dmpdb.c core/src/txt_tok_lib.c \
		-I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o bin/dmpdb
dbget_lso: tests/dbget.c bin/libkvdb.so | core/include core/src core/lib bin tests
	gcc $(WARNING_FLAGS) -fPIC -O2 -s tests/dbget.c core/src/txt_tok_lib.c \
		-I./core/include -Lbin -lkvdb \
		-Wl,-rpath=./bin -o bin/dbget