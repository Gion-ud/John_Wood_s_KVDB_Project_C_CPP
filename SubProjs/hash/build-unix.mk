all: bin/libhash_table_lib.so bin/libkv_dat_lib.so bin/main

HT_LIB_SRC = src/hash_func_module.c src/hash_table_lib.c

bin/libhash_table_lib.so: $(HT_LIB_SRC)
	gcc -shared -O2 -s -fPIC -Iinclude $(HT_LIB_SRC) -o $@

bin/libkv_dat_lib.so: src/kv_dat_lib.c def/kv_dat_lib.def
	gcc -shared -O2 -s -Iinclude src/kv_dat_lib.c -o $@


bin/main: bin/libkv_dat_lib.so tests/main.c
	gcc -O2 -s -fPIC tests/main.c -Iinclude -Lbin -lhash_table_lib -Wl,-rpath=./bin -o bin/main
