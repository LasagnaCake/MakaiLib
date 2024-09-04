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
	$(COMPILER) $(DEBUGCOMPCFG) -c vertex.cpp -o graph.gl.vertex.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c uniform.cpp -o graph.gl.uniform.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c blend.cpp -o graph.gl.blend.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c color.cpp -o graph.gl.color.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c image.cpp -o graph.gl.image.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c texture.cpp -o graph.gl.texture.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c shader.cpp -o graph.gl.shader.$@.o
	
	@cd renderer
	@make $@
	@cd ..

	@cd material
	@make $@
	@cd ..


release:
	$(COMPILER) $(COMPCFG) -c vertex.cpp -o graph.gl.vertex.$@.o
	$(COMPILER) $(COMPCFG) -c uniform.cpp -o graph.gl.uniform.$@.o
	$(COMPILER) $(COMPCFG) -c blend.cpp -o graph.gl.blend.$@.o
	$(COMPILER) $(COMPCFG) -c color.cpp -o graph.gl.color.$@.o
	$(COMPILER) $(COMPCFG) -c image.cpp -o graph.gl.image.$@.o
	$(COMPILER) $(COMPCFG) -c texture.cpp -o graph.gl.texture.$@.o
	$(COMPILER) $(COMPCFG) -c shader.cpp -o graph.gl.shader.$@.o
	
	@cd renderer
	@make $@
	@cd ..

	@cd material
	@make $@
	@cd ..
