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

lower = $(shell echo $(1) | tr A-Z a-z)
upper = $(shell echo $(1) | tr a-z A-Z)

ifndef gl-loader
gl-loader := glad
endif

override prefix := $(strip $(prefix)).mat

OPTIMIZATIONS	:= $(USE_OPENMP) -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize $(LEAN)

INCLUDES := -I$(ROOT)/lib/SDL2-2.0.10/include -I$(ROOT)/lib/OpenGL -I$(ROOT)/lib/OpenGL/$(call upper,$(gl-loader))/include

DEBUGMODE := -DMAKAILIB_DEBUG -DCTL_DEBUG -DNDEBUG
DEBUGCOMPCFG	:= $(COMPILER_CONFIG) -Wall -Wpedantic -Og -ggdb3 -fno-omit-frame-pointer $(DEBUGMODE)
DEBUGLINKCFG	:= $(LINKER_CONFIG) -Og $(LIBRARIES)

ifdef debug-release
DEBUGRELEASE := $(DEBUGMODE)
export debug-release
else
DEBUGRELEASE := -DNDEBUG
endif

COMPCFG	:= $(COMPILER_CONFIG) -O3 $(OPTIMIZATIONS) $(DEBUGRELEASE)
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
	$(COMPILER) $(DEBUGCOMPCFG) -c effect.cpp -o $(prefix).effect.gl.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c materials.cpp -o $(prefix).materials.gl.$@.o


release:
	$(COMPILER) $(COMPCFG) -c effect.cpp -o $(prefix).effect.$@.o
	$(COMPILER) $(COMPCFG) -c materials.cpp -o $(prefix).materials.$@.o
