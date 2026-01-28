all: main

main: main.c kv_lib.c
	gcc -O2 -s main.c kv_lib.c -o main
	./main

read: read.c kv_lib.c
	gcc -O2 -s read.c kv_lib.c -o read
	./read