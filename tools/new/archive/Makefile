CC 	?= gcc
CXX ?= g++

ROOT	:= ../../..

COMPILER_CONFIG	:= -fexpensive-optimizations -m64 -std=gnu++20 -fcoroutines -fms-extensions -DENABLE_DEBUG_OUTPUT_ -DNDEBUG -static
LINKER_CONFIG	:= -static-libstdc++ -static-libgcc -static -m64 -fms-extensions -s

OPTIMIZATIONS	:= -fopenmp -openmp -ftree-parallelize-loops=128 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize

WINGARBAGE	:= -lole32 -loleaut32 -limm32 -lwinmm -lversion -lpowrprof -lcomdlg32 -lsetupapi -lgdi32

INCLUDES	:= -I$(ROOT)/lib/cryptopp/include -I$(ROOT)/lib/cppcodec-0.2 -I$(ROOT)/src/new/tool/archive
LIBRARIES	:= $(WINGARBAGE) $(ROOT)/lib/cryptopp/lib/libcryptopp.a -lopengl32 -lgomp

DEBUGCOMPCFG	:= $(COMPILER_CONFIG) -Wall -Wpedantic -pg -Og -ggdb3 -fno-omit-frame-pointer
DEBUGLINKCFG	:= $(LINKER_CONFIG) -pg -Og $(LIBRARIES)

COMPCFG	:= $(COMPILER_CONFIG) -O3 $(OPTIMIZATIONS) -D_DEBUG_OUTPUT_
LINKCFG	:= $(LINKER_CONFIG) -s -O3 $(LIBRARIES)

COMPILER	:= @$(CXX) $(INCLUDES)
LINKER		:= @$(CXX)

EXTRAOBJS := $(ROOT)/src/new/file/json.o $(ROOT)/src/new/tool/archive/archive.o

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
	@echo ""
	@echo "[--- START ---]"
	$(GET_TIME)
	
	@echo "[1/2] compiling [$@]..."
	$(COMPILER) $(DEBUGCOMPCFG) -c arcpack.cpp -o arcpack_debug.o
	$(COMPILER) $(DEBUGCOMPCFG) -c arcunpack.cpp -o arcunpack_debug.o
	$(COMPILER) $(DEBUGCOMPCFG) -c arcgen.cpp -o arcgen_debug.o

	$(LINKER) -o arcpack_debug.exe $(EXTRAOBJS) arcpack_debug.o $(LINKCFG)
	$(LINKER) -o arcunpack_debug.exe $(EXTRAOBJS) arcunpack_debug.o $(LINKCFG)
	$(LINKER) -o arcgen_debug.exe $(EXTRAOBJS) arcgen_debug.o $(LINKCFG)

	@echo "Done!"
	
	$(GET_TIME)
	@echo "[--- END ---]"
	@echo 


release:
	@echo ""
	@echo "[--- START ---]"
	$(GET_TIME)
	
	@echo "[1/2] compiling [$@]..."
	$(COMPILER) $(COMPCFG) -c arcpack.cpp -o arcpack.o
	$(COMPILER) $(COMPCFG) -c arcunpack.cpp -o arcunpack.o
	$(COMPILER) $(COMPCFG) -c arcgen.cpp -o arcgen.o

	$(LINKER) -o arcpack.exe $(EXTRAOBJS) arcpack.o $(LINKCFG)
	$(LINKER) -o arcunpack.exe $(EXTRAOBJS) arcunpack.o $(LINKCFG)
	$(LINKER) -o arcgen.exe $(EXTRAOBJS) arcgen.o $(LINKCFG)

	@echo "Done!"
	
	$(GET_TIME)
	@echo "[--- END ---]"
	@echo 