mkdb-ldll: mkdb.c ../release-build/KVDB/bin/kvdb_lib.dll | ../release-build/KVDB/bin ../release-build/KVDB/lib
	gcc mkdb.c -O2 -s -I../release-build/KVDB/include -L../release-build/KVDB/lib -lkvdb_lib -o ../release-build/KVDB/bin/mkdb

rddb-ldll: rddb.c ../release-build/KVDB/bin/kvdb_lib.dll | ../release-build/KVDB/bin ../release-build/KVDB/lib
	gcc rddb.c -O2 -s -I../release-build/KVDB/include -L../release-build/KVDB/lib -lkvdb_lib -o ../release-build/KVDB/bin/rddb

mkdb-lso: mkdb.c ../release-build/KVDB/bin/libkvdb_lib.so | ../release-build/KVDB/bin
	gcc mkdb.c -O2 -s -fPIC \
	-I../release-build/KVDB/include \
	-L../release-build/KVDB/bin -lkvdb_lib \
	-Wl,-rpath=../release-build/KVDB/bin \
	-o ../release-build/KVDB/bin/mkdb

rddb-lso: rddb.c ../release-build/KVDB/bin/libkvdb_lib.so | ../release-build/KVDB/bin
	gcc rddb.c -O2 -s -fPIC \
		-I../release-build/KVDB/include \
		-L../release-build/KVDB/bin -lkvdb_lib \
		-Wl,-rpath=../release-build/KVDB/bin \
		-o ../release-build/KVDB/bin/rddb

new-db:
	../release-build/KVDB/bin/mkdb ../res/Pokemon.csv 1 database/table0001.db

read-db:
	../release-build/KVDB/bin/rddb database/table0001.db logs/table0001.db.dmp.txt

clean:
	rm \
		../release-build/KVDB/bin/*.exe \
		../release-build/KVDB/bin/mkdb \
		../release-build/KVDB/bin/rddb \
		logs/*
