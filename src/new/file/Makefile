CC 	?= gcc
CXX ?= g++

LEAN := -static -s

COMPILER_CONFIG	:= -fexpensive-optimizations -m64 -std=gnu++20 -fconcepts -fcoroutines -fms-extensions
LINKER_CONFIG	:= -static-libstdc++ -static-libgcc -m64 -fms-extensions

ifdef openmp
USE_OPENMP := -fopenmp -openmp -ftree-parallelize-loops=128
export openmp
endif

prefix += .file
export prefix

OPTIMIZATIONS	:= $(USE_OPENMP) -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize $(LEAN)

DEBUGMODE := -DMAKAILIB_DEBUG -DCTL_DEBUG -DNDEBUG
DEBUGCOMPCFG	:= $(COMPILER_CONFIG) -Wall -Wpedantic -Og -ggdb3 -fno-omit-frame-pointer $(DEBUGMODE)
DEBUGLINKCFG	:= $(LINKER_CONFIG) -Og $(LIBRARIES)

COMPCFG	:= $(COMPILER_CONFIG) -O3 $(OPTIMIZATIONS)
LINKCFG	:= $(LINKER_CONFIG) -O3 $(LIBRARIES) $(LEAN)

COMPILER	:= @$(CXX) $(INCLUDES)
LINKER		:= @$(CXX)

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
	$(COMPILER) $(DEBUGCOMPCFG) -c json.cpp -o $(prefix).json.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c slf.cpp -o $(prefix).slf.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c get.cpp -o $(prefix).get.$@.o


release:
	$(COMPILER) $(COMPCFG) -c json.cpp -o file.json.$@.o
	$(COMPILER) $(COMPCFG) -c slf.cpp -o file.slf.$@.o
	$(COMPILER) $(COMPCFG) -c get.cpp -o file.get.$@.o
