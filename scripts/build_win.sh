#!/usr/bin/bash

make clean
make build/db_lib.o build/hash_table_lib.o
make kvdb_dll

