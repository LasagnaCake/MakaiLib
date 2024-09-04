CC 	?= gcc
CXX ?= g++

LEAN := -static -s

ROOT	:= ../../../..

COMPILER_CONFIG	:= -fexpensive-optimizations -m64 -std=gnu++20 -fconcepts -fcoroutines -fms-extensions
LINKER_CONFIG	:= -static-libstdc++ -static-libgcc -m64 -fms-extensions

OPTIMIZATIONS	:= -fopenmp -openmp -ftree-parallelize-loops=128 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize $(LEAN)

INCLUDES := -I$(ROOT)/lib/SDL2-2.0.10/include -I$(ROOT)/lib/OpenGL -I$(ROOT)/lib/stb

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
	$(COMPILER) $(DEBUGCOMPCFG) -c vertex.cpp -o vertex.gl.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c uniform.cpp -o uniform.gl.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c blend.cpp -o blend.gl.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c color.cpp -o color.gl.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c image.cpp -o image.gl.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c texture.cpp -o texture.gl.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c shader.cpp -o shader.gl.$@.o
	
	@cd renderer
	@make $@
	@cd ..

	@cd material
	@make $@
	@cd ..


release:
	$(COMPILER) $(COMPCFG) -c vertex.cpp -o vertex.gl.$@.o
	$(COMPILER) $(COMPCFG) -c uniform.cpp -o uniform.gl.$@.o
	$(COMPILER) $(COMPCFG) -c blend.cpp -o blend.gl.$@.o
	$(COMPILER) $(COMPCFG) -c color.cpp -o color.gl.$@.o
	$(COMPILER) $(COMPCFG) -c image.cpp -o image.gl.$@.o
	$(COMPILER) $(COMPCFG) -c texture.cpp -o texture.gl.$@.o
	$(COMPILER) $(COMPCFG) -c shader.cpp -o shader.gl.$@.o
	
	@cd renderer
	@make $@
	@cd ..

	@cd material
	@make $@
	@cd ..
