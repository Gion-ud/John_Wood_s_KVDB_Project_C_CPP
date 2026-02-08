all: bin/hash_table_lib.dll bin/kv_dat_lib.dll bin/main

HT_LIB_SRC = src/hash_func_module.c src/hash_table_lib.c

HASH_INDEX_LIB_SRC = src/hash_func_module.c src/hash_index_lib.c

bin/hash_table_lib.dll: $(HT_LIB_SRC) def/hash_table_lib.def
	gcc -shared -O2 -s -Iinclude $(HT_LIB_SRC) def/hash_table_lib.def -o $@ \
		-Wl,--output-def,lib/hash_table_lib_exports.def \
		-Wl,--kill-at \
		-Wl,--out-implib,lib/libhash_table_lib.dll.a

bin/hash_index_lib.dll: $(HASH_INDEX_LIB_SRC) def/hash_index_lib.def
	gcc -shared -O2 -s -Iinclude $(HASH_INDEX_LIB_SRC) def/hash_index_lib.def -o $@ \
		-Wl,--output-def,lib/hash_index_lib_exports.def \
		-Wl,--kill-at \
		-Wl,--out-implib,lib/libhash_index_lib.dll.a


bin/kv_dat_lib.dll: src/kv_dat_lib.c def/kv_dat_lib.def
	gcc -shared -O2 -s -Iinclude src/kv_dat_lib.c def/kv_dat_lib.def -o $@ \
		-Wl,--output-def,lib/kv_dat_lib_exports.def \
		-Wl,--kill-at \
		-Wl,--out-implib,lib/kv_dat_lib.dll.a


bin/main: bin/hash_table_lib.dll tests/main.c
	gcc -O2 -s tests/main.c -Iinclude -Llib -lhash_table_lib -o bin/main

