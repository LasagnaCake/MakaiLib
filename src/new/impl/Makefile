CC 	?= gcc
CXX ?= g++

LEAN := -static -s

ROOT	:= ../../..

C_COMPCFG		:= -fexpensive-optimizations -m64 -fms-extensions
COMPILER_CONFIG	:= -fexpensive-optimizations -m64 -std=gnu++20 -fconcepts -fcoroutines -fms-extensions
LINKER_CONFIG	:= -static-libstdc++ -static-libgcc -m64 -fms-extensions

ifdef openmp
USE_OPENMP := -fopenmp -openmp -ftree-parallelize-loops=128
export openmp
endif

override prefix := $(strip $(prefix)).impl

OPTIMIZATIONS	:= $(USE_OPENMP) -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize $(LEAN)

INCLUDES := -I$(ROOT)/lib/stb

DEBUGMODE := -DMAKAILIB_DEBUG -DCTL_DEBUG -DNDEBUG
DEBUGCOMPCFG	:= $(COMPILER_CONFIG) -Wall -Og -ggdb3 -fno-omit-frame-pointer $(DEBUGMODE)
DEBUGLINKCFG	:= $(LINKER_CONFIG) -Og $(LIBRARIES)

ifdef debug-release
RELEASEMODE := $(DEBUGMODE)
export debug-release
endif

COMPCFG	:= $(COMPILER_CONFIG) -O3 $(OPTIMIZATIONS) $(RELEASEMODE)
LINKCFG	:= $(LINKER_CONFIG) -O3 $(LIBRARIES) $(LEAN)

DEBUGCOMPCFG_C	:= $(C_COMPCFG) -Wall -Og -ggdb3 -fno-omit-frame-pointer $(DEBUGMODE)
COMPCFG_C		:= $(C_COMPCFG) -O3 $(OPTIMIZATIONS)

COMPILER	:= @$(CXX) $(INCLUDES)
LINKER		:= @$(CXX)

lower = $(shell echo $(1) | tr A-Z a-z)
upper = $(shell echo $(1) | tr a-z A-Z)

ifdef gl-loader
ifeq ($(gl-loader),glew)
MAKE_GLOADER_D	:= @:
MAKE_GLOADER	:= @:
else
MAKE_GLOADER_D	= $(CC) -I$(ROOT)/lib/OpenGL/$(call upper,$(gl-loader))/include $(DEBUGCOMPCFG_C) -c $(gl-loader).c -o $(prefix).glapi.$@.o
MAKE_GLOADER	= $(CC) -I$(ROOT)/lib/OpenGL/$(call upper,$(gl-loader))/include $(COMPCFG_C) -c $(gl-loader).c -o $(prefix).glapi.$@.o
endif
else
gl-loader := glad
MAKE_GLOADER_D	= $(CC) -I$(ROOT)/lib/OpenGL/$(call upper,$(gl-loader))/include $(DEBUGCOMPCFG_C) -c $(gl-loader).c -o $(prefix).glapi.$@.o
MAKE_GLOADER	= $(CC) -I$(ROOT)/lib/OpenGL/$(call upper,$(gl-loader))/include $(COMPCFG_C) -c $(gl-loader).c -o $(prefix).glapi.$@.o
endif

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
	$(COMPILER) $(DEBUGCOMPCFG) -Wpedantic -c stb.cpp -o $(prefix).stb.$@.o
	$(MAKE_GLOADER_D)


release:
	$(COMPILER) $(COMPCFG) -c stb.cpp -o $(prefix).stb.$@.o
	$(MAKE_GLOADER)
