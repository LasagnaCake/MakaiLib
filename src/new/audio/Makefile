CC 	?= gcc
CXX ?= g++

LEAN := -static -s

ROOT	:= ../../..

COMPILER_CONFIG	:= -fexpensive-optimizations -m64 -std=gnu++20 -fconcepts -fcoroutines -fms-extensions
LINKER_CONFIG	:= -static-libstdc++ -static-libgcc -m64 -fms-extensions

OPTIMIZATIONS	:= -fopenmp -openmp -ftree-parallelize-loops=128 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize $(LEAN)

INCLUDES := -I$(ROOT)/lib/SDL2-2.0.10/include

DEBUGMODE := -DENABLE_DEBUG_OUTPUT_ -D_DEBUG_OUTPUT_ -DNDEBUG
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
	$(COMPILER) $(DEBUGCOMPCFG) -c core.cpp -o audio.core.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c playable.cpp -o audio.playable.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c music.cpp -o audio.music.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c sound.cpp -o audio.sound.$@.o


release:
	$(COMPILER) $(COMPCFG) -c core.cpp -o audio.core.$@.o
	$(COMPILER) $(COMPCFG) -c playable.cpp -o audio.playable.$@.o
	$(COMPILER) $(COMPCFG) -c music.cpp -o audio.music.$@.o
	$(COMPILER) $(COMPCFG) -c sound.cpp -o audio.sound.$@.o