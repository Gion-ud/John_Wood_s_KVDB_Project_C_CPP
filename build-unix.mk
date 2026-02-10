build-bin: add_bin_to_path bin/libkvdb_lib.so mktbldb_lso dmptbldb_lso getdbrec_lso

add_bin_to_path:
	echo 'export PATH="$$PATH:./bin"'


WARNING_FLAGS = -Wall -Wextra# -Werror

MODULE_OBJ = build/db_lib.o build/hash_func_module.o build/hash_index_lib.o build/global_func.o

# dll for kvdb
bin/libkvdb_lib.so: $(MODULE_OBJ) | build core/src core/include bin
	gcc $(WARNING_FLAGS) -O2 -s -shared -fPIC $(MODULE_OBJ) -I./core/include -o bin/libkvdb_lib.so


mktbldb_lso: tests/mktbldb.c bin/libkvdb_lib.so build/txt_tok_lib.o build/global_func.o | core/include core/src bin tests
	gcc $(WARNING_FLAGS) -fPIC -O2 -s \
		tests/mktbldb.c build/txt_tok_lib.o build/global_func.o \
		-I./core/include -Lbin -lkvdb_lib -Wl,-rpath=./bin -o bin/mktbldb

# -g -fsanitize=address

dmptbldb_lso: tests/dmptbldb.c bin/libkvdb_lib.so build/txt_tok_lib.o build/global_func.o | core/include core/src bin tests
	gcc $(WARNING_FLAGS) -fPIC -O2 -s \
		tests/dmptbldb.c build/txt_tok_lib.o build/global_func.o \
		-I./core/include -Lbin -lkvdb_lib -Wl,-rpath=./bin -o bin/dmptbldb

getdbrec_lso: tests/getdbrec.c bin/libkvdb_lib.so | core/include core/src core/lib bin tests
	gcc $(WARNING_FLAGS) -fPIC -O2 -s tests/getdbrec.c \
		-I./core/include -Lbin -lkvdb_lib \
		-Wl,-rpath=./bin -o bin/getdbrec