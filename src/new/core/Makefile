CC 	?= gcc
CXX ?= g++

LEAN := -static -s

ROOT	:= ../../..

ifdef no-buffers
NO_BUFFERS := -DMAKAILIB_DO_NOT_USE_BUFFERS
export no-buffers
endif

COMPILER_CONFIG	:= -fexpensive-optimizations -m64 -std=gnu++20 -fconcepts -fcoroutines -fms-extensions $(NO_BUFFERS)
LINKER_CONFIG	:= -static-libstdc++ -static-libgcc -m64 -fms-extensions

ifdef openmp
USE_OPENMP := -fopenmp -openmp -ftree-parallelize-loops=128
export openmp
endif

override prefix := $(strip $(prefix)).core

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
	$(COMPILER) $(DEBUGCOMPCFG) -c app.cpp -o $(prefix).app.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c display.cpp -o $(prefix).display.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c dpi.cpp -o $(prefix).dpi.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c popup.cpp -o $(prefix).popup.$@.o

	@cd input
	@make $@ prefix="$(prefix)"
	@cd ..


release:
	$(COMPILER) $(COMPCFG) -c app.cpp -o $(prefix).app.$@.o
	$(COMPILER) $(COMPCFG) -c display.cpp -o $(prefix).display.$@.o
	$(COMPILER) $(COMPCFG) -c dpi.cpp -o $(prefix).dpi.$@.o
	$(COMPILER) $(COMPCFG) -c popup.cpp -o $(prefix).popup.$@.o

	@cd input
	@make $@ prefix="$(prefix)"
	@cd ..
