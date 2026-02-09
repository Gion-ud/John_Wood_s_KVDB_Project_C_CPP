

mkdb_ldll: tests/mkdb.c bin/kvdb_lib.dll | core/include core/src core/lib bin tests
	gcc tests/mkdb.c -O2 -s -I./core/include -L./core/lib -lkvdb_lib -o bin/mkdb

rddb_ldll: tests/rddb.c bin/kvdb_lib.dll | core/include core/src core/lib bin tests
	gcc tests/rddb.c -O2 -s -I./core/include -L./core/lib -lkvdb_lib -o bin/rddb

getdbrec_ldll: tests/getdbrec.c bin/kvdb_lib.dll | core/include core/src core/lib bin tests
	gcc tests/getdbrec.c -O2 -s -I./core/include -L./core/lib -lkvdb_lib -o bin/get-db-rec
