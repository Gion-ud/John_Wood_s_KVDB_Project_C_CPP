all: build

ifeq ($(OS), Windows_NT)
RC = $(MAKE) -f build-win.mk
else
RC = $(MAKE) -f build-unix.mk
endif

build:
	$(RC)

run:
	./bin/main

gen-uuid: scripts/gen-uuid.py | scripts
	python3 scripts/gen-uuid.py uuid.txt 32

clean: 
	rm bin/* lib/*
