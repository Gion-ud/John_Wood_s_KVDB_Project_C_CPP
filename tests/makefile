../release-build/KVDB/bin/mkdb: mkdb.c ../release-build/KVDB/bin/kvdb_lib.dll | ../release-build/KVDB/bin ../release-build/KVDB/lib
	gcc mkdb.c -O2 -s -I../release-build/KVDB/include -L../release-build/KVDB/lib -lkvdb_lib -o ../release-build/KVDB/bin/mkdb

../release-build/KVDB/bin/read: read.c ../release-build/KVDB/bin/kvdb_lib.dll | ../release-build/KVDB/bin ../release-build/KVDB/lib
	gcc read.c -O2 -s -I../release-build/KVDB/include -L../release-build/KVDB/lib -lkvdb_lib -o ../release-build/KVDB/bin/read

new-db: ../release-build/KVDB/bin/mkdb
	../release-build/KVDB/bin/mkdb ../res/Pokemon.csv database/table0001.db

read-db: ../release-build/KVDB/bin/read
	../release-build/KVDB/bin/read database/table0001.db table0001.db.dmp.txt

clean:
	rm \
		../release-build/KVDB/bin/*.exe \
		../release-build/KVDB/bin/mkdb \
		../release-build/KVDB/bin/read