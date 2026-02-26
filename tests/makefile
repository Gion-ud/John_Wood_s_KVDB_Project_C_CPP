all: main

CXX = g++

main: main.cpp ../lib/libkvdb.dll.a ../external/zlib-win-x64/zdll.lib
	$(CXX) -O3 -s -I../core/include -I../external/zlib-win-x64/include \
		$< -o $@ -L../lib -lkvdb -L../external/zlib-win-x64 -lzdll
	# export PATH="$$PATH:../lib"
	# export PATH="$$PATH:../external/zlib-win-x64"

decompr: decompr.cpp ../lib/libkvdb.dll.a ../external/zlib-win-x64/zdll.lib
	$(CXX) -O3 -s -I../core/include -I../external/zlib-win-x64/include \
		$< -o $@ -L../lib -lkvdb -L../external/zlib-win-x64 -lzdll


