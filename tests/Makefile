CC	:= gcc
CXX	:= g++

COMPILER_CONFIG	:= -m64 -std=gnu++20 -fms-extensions -I../output/include
DEBUGMODE		:= -D_DEBUG_OUTPUT_

all: debug release

debug:
	$(CXX) -o compilation.$@.exe compilation.cpp $(COMPILER_CONFIG) ../output/lib/libmakai.debug.a $(DEBUGMODE)

debug:
	$(CXX) -o compilation.exe compilation.cpp $(COMPILER_CONFIG) ../output/lib/libmakai.a -mwindows
