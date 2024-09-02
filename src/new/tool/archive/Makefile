CC 	?= gcc
CXX ?= g++

ROOT	:= ../../../..

LEAN := -static -s

COMPILER_CONFIG	:= -fexpensive-optimizations -m64 -std=gnu++20 -fcoroutines -fms-extensions
LINKER_CONFIG	:= -static-libstdc++ -static-libgcc -m64 -fms-extensions

OPTIMIZATIONS	:= -fopenmp -openmp -ftree-parallelize-loops=128 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize $(LEAN)

WINGARBAGE	:= -lole32 -loleaut32 -limm32 -lwinmm -lversion -lpowrprof -lcomdlg32 -lsetupapi -lgdi32

INCLUDES	:= -I$(ROOT)/lib/cryptopp/include -I$(ROOT)/lib/cppcodec-0.2
LIBRARIES	:= $(WINGARBAGE) $(ROOT)/lib/cryptopp/lib/libcryptopp.a -lopengl32 -lgomp

DEBUGMODE := -DENABLE_DEBUG_OUTPUT_ -D_DEBUG_OUTPUT_ -DNDEBUG
DEBUGCOMPCFG	:= $(COMPILER_CONFIG) -Wall -Wpedantic -Og -ggdb3 -fno-omit-frame-pointer $(DEBUGMODE)
DEBUGLINKCFG	:= $(LINKER_CONFIG) -Og $(LIBRARIES) 

COMPCFG	:= $(COMPILER_CONFIG) -O3 $(OPTIMIZATIONS)
LINKCFG	:= $(LINKER_CONFIG) -s -O3 $(LIBRARIES) $(LEAN)

COMPILER	:= @$(CXX) $(INCLUDES)
LINKER		:= @$(CXX)

EXTRAOBJS := ../../file/json.o

LIBNAME	:= archive

define GET_TIME
@printf "\nTime: "
@date +\"%H:%M:%S\"
@echo ""
endef

.PHONY: debug release all
.ONESHELL:
.SHELLFLAGS = -ec

all: debug release

debug:
	$(COMPILER) $(DEBUGCOMPCFG) -c $(LIBNAME).cpp -o $(LIBNAME).$@.o


release:
	$(COMPILER) $(COMPCFG) -c $(LIBNAME).cpp -o $(LIBNAME).$@.o