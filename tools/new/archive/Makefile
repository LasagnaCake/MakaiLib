CC 	?= gcc
CXX ?= g++

ROOT	:= ../../..

LEAN := -static -s

COMPILER_CONFIG	:= -fexpensive-optimizations -m64 -std=gnu++20 -fcoroutines -fms-extensions
LINKER_CONFIG	:= -static-libstdc++ -static-libgcc -m64 -fms-extensions

ifdef openmp
USE_OPENMP := -fopenmp -openmp -ftree-parallelize-loops=128
export openmp
endif

export o?=3

OPTIMIZATIONS	:= $(USE_OPENMP) -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize $(LEAN)

WINGARBAGE	:= -lole32 -loleaut32 -limm32 -lwinmm -lversion -lpowrprof -lcomdlg32 -lsetupapi -lgdi32

INCLUDES	:= -I$(ROOT)/lib/cryptopp/include -I$(ROOT)/lib/cppcodec-0.2 -I$(ROOT)/src/new/tool/archive
LIBRARIES	:= $(WINGARBAGE) $(ROOT)/lib/cryptopp/lib/libcryptopp.a

DEBUGMODE		:= -DMAKAILIB_DEBUG -DCTL_DEBUG -DNDEBUG
DEBUGCOMPCFG	:= $(COMPILER_CONFIG) -Wall -Wpedantic -Og -ggdb3 -fno-omit-frame-pointer $(DEBUGMODE)
DEBUGLINKCFG	:= $(LINKER_CONFIG) -Og $(LIBRARIES) -lgcc -lstdc++

COMPCFG	:= $(COMPILER_CONFIG) -O$(o) $(OPTIMIZATIONS) $(DEBUGMODE)
LINKCFG	:= $(LINKER_CONFIG) -s -O$(o) $(LIBRARIES) $(LEAN)

COMPILER	:= @$(CXX) $(INCLUDES)
LINKER		:= @$(CXX)

DEBUGEXTRAOBJS := $(ROOT)/output/lib/libmakai.debug.a
EXTRAOBJS := $(ROOT)/output/lib/libmakai.a

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
	$(COMPILER) $(DEBUGCOMPCFG) -c arcpack.cpp -o arcpack.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c arcunpack.cpp -o arcunpack.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c arcgen.cpp -o arcgen.$@.o

	$(LINKER) -o arcpack.$@.exe arcpack.$@.o $(DEBUGEXTRAOBJS) $(LINKCFG)
	$(LINKER) -o arcunpack.$@.exe arcunpack.$@.o $(DEBUGEXTRAOBJS) $(LINKCFG)
	$(LINKER) -o arcgen.$@.exe arcgen.$@.o $(DEBUGEXTRAOBJS) $(LINKCFG)

	@echo "Done!"
	
	$(GET_TIME)
	@echo "[--- END ---]"
	@echo 


release:
	@echo ""
	@echo "[--- START ---]"
	$(GET_TIME)
	
	@echo "[1/2] compiling [$@]..."
	$(COMPILER) $(COMPCFG) -c arcpack.cpp -o arcpack.$@.o
	$(COMPILER) $(COMPCFG) -c arcunpack.cpp -o arcunpack.$@.o
	$(COMPILER) $(COMPCFG) -c arcgen.cpp -o arcgen.$@.o

	$(LINKER) -o arcpack.exe $(EXTRAOBJS) arcpack.$@.o $(LINKCFG)
	$(LINKER) -o arcunpack.exe $(EXTRAOBJS) arcunpack.$@.o $(LINKCFG)
	$(LINKER) -o arcgen.exe $(EXTRAOBJS) arcgen.$@.o  $(LINKCFG)

	@echo "Done!"
	
	$(GET_TIME)
	@echo "[--- END ---]"
	@echo 