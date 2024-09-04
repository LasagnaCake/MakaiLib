CC 	?= gcc
CXX ?= g++

LEAN := -static -s

ROOT	:= ../../../../..

COMPILER_CONFIG	:= -fexpensive-optimizations -m64 -std=gnu++20 -fconcepts -fcoroutines -fms-extensions
LINKER_CONFIG	:= -static-libstdc++ -static-libgcc -m64 -fms-extensions

ifdef openmp
USE_OPENMP := -fopenmp -openmp -ftree-parallelize-loops=128
export openmp
endif

prefix += .rd
export prefix

OPTIMIZATIONS	:= $(USE_OPENMP) -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize $(LEAN)

INCLUDES := -I$(ROOT)/lib/SDL2-2.0.10/include -I$(ROOT)/lib/OpenGL

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
	$(COMPILER) $(DEBUGCOMPCFG) -c mode.cpp -o $(prefix).mode.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c core.cpp -o $(prefix).core.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c drawable.cpp -o $(prefix).drawable.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c reference.cpp -o $(prefix).reference.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c renderable.cpp -o $(prefix).renderable.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c bar.cpp -o $(prefix).bar.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c text.cpp -o $(prefix).text.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c scene.cpp -o $(prefix).scene.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c framebuffer.cpp -o $(prefix).framebuffer.$@.o


release:
	$(COMPILER) $(COMPCFG) -c mode.cpp -o $(prefix).mode.$@.o
	$(COMPILER) $(COMPCFG) -c core.cpp -o $(prefix).core.$@.o
	$(COMPILER) $(COMPCFG) -c drawable.cpp -o $(prefix).drawable.$@.o
	$(COMPILER) $(COMPCFG) -c reference.cpp -o $(prefix).reference.$@.o
	$(COMPILER) $(COMPCFG) -c renderable.cpp -o $(prefix).renderable.$@.o
	$(COMPILER) $(COMPCFG) -c bar.cpp -o $(prefix).bar.$@.o
	$(COMPILER) $(COMPCFG) -c text.cpp -o $(prefix).text.$@.o
	$(COMPILER) $(COMPCFG) -c scene.cpp -o $(prefix).scene.$@.o
	$(COMPILER) $(COMPCFG) -c framebuffer.cpp -o $(prefix).framebuffer.$@.o
