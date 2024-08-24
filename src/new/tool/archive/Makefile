CC 	?= gcc
CXX ?= g++

ROOT	:= ../../../..

COMPILER_CONFIG	:= -fexpensive-optimizations -m64 -std=gnu++20 -fcoroutines -fms-extensions -DENABLE_DEBUG_OUTPUT_ -DNDEBUG -static
LINKER_CONFIG	:= -static-libstdc++ -static-libgcc -static -m64 -fms-extensions -s

OPTIMIZATIONS	:= -fopenmp -openmp -ftree-parallelize-loops=128 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize

WINGARBAGE	:= -lole32 -loleaut32 -limm32 -lwinmm -lversion -lpowrprof -lcomdlg32 -lsetupapi -lgdi32

INCLUDES	:= -I$(ROOT)/lib/cryptopp/include -I$(ROOT)/lib/cppcodec-0.2
LIBRARIES	:= $(WINGARBAGE) $(ROOT)/lib/cryptopp/lib/libcryptopp.a -lopengl32 -lgomp

DEBUGCOMPCFG	:= $(COMPILER_CONFIG) -Wall -Wpedantic -pg -Og -ggdb3 -fno-omit-frame-pointer
DEBUGLINKCFG	:= $(LINKER_CONFIG) -pg -Og $(LIBRARIES)

COMPCFG	:= $(COMPILER_CONFIG) -O3 $(OPTIMIZATIONS) -D_DEBUG_OUTPUT_
LINKCFG	:= $(LINKER_CONFIG) -s -O3 $(LIBRARIES)

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
	@echo ""
	@echo "[--- START ---]"
	$(GET_TIME)
	
	@echo "[1/2] compiling [$@]..."
	$(COMPILER) $(DEBUGCOMPCFG) -c $(LIBNAME).cpp -o $(LIBNAME)_debug.o

	@echo "Done!"
	
	$(GET_TIME)
	@echo "[--- END ---]"
	@echo 


release:
	@echo ""
	@echo "[--- START ---]"
	$(GET_TIME)
	
	@echo "[1/2] compiling [$@]..."
	$(COMPILER) $(COMPCFG) -c $(LIBNAME).cpp -o $(LIBNAME).o

	@echo "Done!"
	
	$(GET_TIME)
	@echo "[--- END ---]"
	@echo 