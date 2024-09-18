CC 	?= gcc
CXX ?= g++

make-prefix := $(strip $(1))

LEAN := -static -s

COMPILER_CONFIG	:= -fexpensive-optimizations -m64 -std=gnu++20 -fconcepts -fcoroutines -fms-extensions

ifdef openmp
USE_OPENMP := -fopenmp -openmp -ftree-parallelize-loops=128
export openmp
endif

OPTIMIZATIONS	:= $(USE_OPENMP) -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize

DEBUGMODE		:= -DMAKAILIB_DEBUG -DCTL_DEBUG -DNDEBUG

ifdef debug-release
RELEASEMODE := $(DEBUGMODE)
export debug-release
else
RELEASEMODE := $(LEAN)
endif

export o?=3

DEBUG_CONFIG	:= $(COMPILER_CONFIG) -O$(o) $(OPTIMIZATIONS) $(RELEASEMODE)
RELEASE_CONFIG	:= $(COMPILER_CONFIG) -Wall -Wpedantic -Og -ggdb3 -fno-omit-frame-pointer $(DEBUGMODE)

COMPILER = @$(CXX) $(INCLUDES)

compile-release	= $(COMPILER) $(DEBUG_CONFIG) -c $(1).cpp -o $(2).$(1).$@.o
compile-debug	= $(COMPILER) $(RELEASE_CONFIG) -c $(1).cpp -o $(2).$(1).$@.o

export compile-release
export compile-debug
