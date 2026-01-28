all: hash_table_lib.dll kv_dat_lib.dll main

hash_table_lib.dll: src/hash_table_lib.c def/hash_table_lib.def
	gcc -O2 -s -shared -o bin/hash_table_lib.dll src/hash_table_lib.c def/hash_table_lib.def \
		-Wl,--output-def,tmp/hash_table_lib_exports.def -Wl,--kill-at -Wl,--out-implib,bin/hash_table_lib.dll.a

kv_dat_lib.dll: src/kv_dat_lib.c def/kv_dat_lib.def
	gcc -O2 -s -shared -o bin/kv_dat_lib.dll src/kv_dat_lib.c def/kv_dat_lib.def \
		-Wl,--output-def,tmp/kv_dat_lib_exports.def -Wl,--kill-at -Wl,--out-implib,bin/kv_dat_lib.dll.a

main: src/main.c bin/hash_table_lib.dll bin/kv_dat_lib.dll
	gcc -O2 -s src/main.c -o bin/main -Lbin -lhash_table_lib -lkv_dat_lib
	rm bin/*.dll.a

run:
	./bin/main

clean: 
	rm bin/* tmp/* cache/*
