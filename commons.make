import globals.make

CC	?= gcc
CXX	?= g++

concat = $(strip $(1)).$(strip $(2))

LEAN := -static -s

COMPILER_CONFIG	:= -fexpensive-optimizations -m64 -std=gnu++20 -fconcepts -fcoroutines -fms-extensions

ifdef openmp
USE_OPENMP := -fopenmp -openmp -ftree-parallelize-loops=128
endif

OPTIMIZATIONS	:= $(USE_OPENMP) -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize

DEBUGMODE		:= -DMAKAILIB_DEBUG -DCTL_DEBUG -DNDEBUG

ifdef debug-release
RELEASEMODE := $(DEBUGMODE)
else
RELEASEMODE := $(LEAN)
endif

DEBUG_CONFIG	:= $(COMPILER_CONFIG) -O$(o) $(OPTIMIZATIONS) $(RELEASEMODE)
RELEASE_CONFIG	:= $(COMPILER_CONFIG) -Wall -Wpedantic -Og -ggdb3 -fno-omit-frame-pointer $(DEBUGMODE)

COMPILER = @$(CXX) $(INCLUDES)

compile-release	= $(COMPILER) $(DEBUG_CONFIG) -c $(1).cpp -o $(prefix).$(1).$@.o
compile-debug	= $(COMPILER) $(RELEASE_CONFIG) -c $(1).cpp -o $(prefix).$(1).$@.o

export compile-release
export compile-debug

compile = \
	$(if $(findstring debug,$@),\
		$(call compile-debug, $(1)),\
		$(call compile-release, $(1))\
	)

define GET_TIME
@printf "\nTime: "
@date +\"%H:%M:%S\"
@echo ""
endef
