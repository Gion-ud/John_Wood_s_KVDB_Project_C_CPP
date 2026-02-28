SHELL := /usr/bin/sh

all:

build-core:
	cd core && $(MAKE)

build-utils:
	cd utils && $(MAKE)

clean:
	rm -rf core/build
	rm -rf utils/build
