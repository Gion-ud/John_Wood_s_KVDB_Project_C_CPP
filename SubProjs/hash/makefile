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

clean: 
	rm bin/* lib/*