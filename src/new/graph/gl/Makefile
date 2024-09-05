CC 	?= gcc
CXX ?= g++

LEAN := -static -s

ROOT	:= ../../../..

COMPILER_CONFIG	:= -fexpensive-optimizations -m64 -std=gnu++20 -fconcepts -fcoroutines -fms-extensions
LINKER_CONFIG	:= -static-libstdc++ -static-libgcc -m64 -fms-extensions

ifdef openmp
USE_OPENMP := -fopenmp -openmp -ftree-parallelize-loops=128
export openmp
endif

override prefix := $(strip $(prefix)).gl

OPTIMIZATIONS	:= $(USE_OPENMP) -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize $(LEAN)

INCLUDES := -I$(ROOT)/lib/SDL2-2.0.10/include -I$(ROOT)/lib/OpenGL -I$(ROOT)/lib/stb

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
	$(COMPILER) $(DEBUGCOMPCFG) -c vertex.cpp -o $(prefix).vertex.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c uniform.cpp -o $(prefix).uniform.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c blend.cpp -o $(prefix).blend.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c color.cpp -o $(prefix).color.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c image.cpp -o $(prefix).image.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c texture.cpp -o $(prefix).texture.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c shader.cpp -o $(prefix).shader.$@.o
	
	@cd renderer
	@make $@ prefix="$(prefix)"
	@cd ..

	@cd material
	@make $@ prefix="$(prefix)"
	@cd ..


release:
	$(COMPILER) $(COMPCFG) -c vertex.cpp -o $(prefix).vertex.$@.o
	$(COMPILER) $(COMPCFG) -c uniform.cpp -o $(prefix).uniform.$@.o
	$(COMPILER) $(COMPCFG) -c blend.cpp -o $(prefix).blend.$@.o
	$(COMPILER) $(COMPCFG) -c color.cpp -o $(prefix).color.$@.o
	$(COMPILER) $(COMPCFG) -c image.cpp -o $(prefix).image.$@.o
	$(COMPILER) $(COMPCFG) -c texture.cpp -o $(prefix).texture.$@.o
	$(COMPILER) $(COMPCFG) -c shader.cpp -o $(prefix).shader.$@.o
	
	@cd renderer
	@make $@ prefix="$(prefix)"
	@cd ..

	@cd material
	@make $@ prefix="$(prefix)"
	@cd ..
