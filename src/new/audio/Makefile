CC 	?= gcc
CXX ?= g++

LEAN := -static -s

ROOT	:= ../../..

COMPILER_CONFIG	:= -fexpensive-optimizations -m64 -std=gnu++20 -fconcepts -fcoroutines -fms-extensions
LINKER_CONFIG	:= -static-libstdc++ -static-libgcc -m64 -fms-extensions

ifdef openmp
USE_OPENMP := -fopenmp -openmp -ftree-parallelize-loops=128
export openmp
endif

override prefix := $(strip $(prefix)).audio

OPTIMIZATIONS	:= $(USE_OPENMP) -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize $(LEAN)

INCLUDES := -I$(ROOT)/lib/SDL2-2.0.10/include

DEBUGMODE := -DMAKAILIB_DEBUG -DCTL_DEBUG -DNDEBUG
DEBUGCOMPCFG	:= $(COMPILER_CONFIG) -Wall -Wpedantic -Og -ggdb3 -fno-omit-frame-pointer $(DEBUGMODE)
DEBUGLINKCFG	:= $(LINKER_CONFIG) -Og $(LIBRARIES)

ifdef debug-release
RELEASEMODE := $(DEBUGMODE)
export debug-release
endif

COMPCFG	:= $(COMPILER_CONFIG) -O3 $(OPTIMIZATIONS) $(RELEASEMODE)
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
	$(COMPILER) $(DEBUGCOMPCFG) -c core.cpp -o $(prefix).core.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c playable.cpp -o $(prefix).playable.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c music.cpp -o $(prefix).music.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c sound.cpp -o $(prefix).sound.$@.o


release:
	$(COMPILER) $(COMPCFG) -c core.cpp -o $(prefix).core.$@.o
	$(COMPILER) $(COMPCFG) -c playable.cpp -o $(prefix).playable.$@.o
	$(COMPILER) $(COMPCFG) -c music.cpp -o $(prefix).music.$@.o
	$(COMPILER) $(COMPCFG) -c sound.cpp -o $(prefix).sound.$@.o
