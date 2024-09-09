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

lower = $(shell echo $(1) | tr A-Z a-z)
upper = $(shell echo $(1) | tr a-z A-Z)

ifndef gl-loader
gl-loader := glad
endif

override prefix := $(strip $(prefix)).core

OPTIMIZATIONS	:= $(USE_OPENMP) -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize $(LEAN)

INCLUDES := -I$(ROOT)/lib/SDL2-2.0.10/include -I$(ROOT)/lib/OpenGL -I$(ROOT)/lib/OpenGL/$(call upper,$(gl-loader))/include

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
