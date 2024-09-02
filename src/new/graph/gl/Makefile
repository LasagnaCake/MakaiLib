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
	@echo ""
	@echo "[--- START ---]"
	$(GET_TIME)
	
	@echo "[1/2] compiling [$@]..."
	$(COMPILER) $(DEBUGCOMPCFG) -c vertex.cpp -o vertex_debug.o
	$(COMPILER) $(DEBUGCOMPCFG) -c uniform.cpp -o uniform_debug.o
	$(COMPILER) $(DEBUGCOMPCFG) -c blend.cpp -o blend_debug.o
	$(COMPILER) $(DEBUGCOMPCFG) -c color.cpp -o color_debug.o
	$(COMPILER) $(DEBUGCOMPCFG) -c image.cpp -o image_debug.o
	$(COMPILER) $(DEBUGCOMPCFG) -c texture.cpp -o texture_debug.o
	$(COMPILER) $(DEBUGCOMPCFG) -c shader.cpp -o shader_debug.o

	@echo ""
	@echo "<subsystems>"

	@cd renderer
	@make $@
	@cd ..

	@cd material
	@make $@
	@cd ..

	@echo "</subsystems>"
	@echo ""

	@echo "Done!"
	
	$(GET_TIME)
	@echo "[--- END ---]"
	@echo 


release:
	@echo ""
	@echo "[--- START ---]"
	$(GET_TIME)
	
	@echo "[1/2] compiling [$@]..."
	$(COMPILER) $(COMPCFG) -c vertex.cpp -o vertex.o
	$(COMPILER) $(COMPCFG) -c uniform.cpp -o uniform.o
	$(COMPILER) $(COMPCFG) -c blend.cpp -o blend.o
	$(COMPILER) $(COMPCFG) -c color.cpp -o color.o
	$(COMPILER) $(COMPCFG) -c image.cpp -o image.o
	$(COMPILER) $(COMPCFG) -c texture.cpp -o texture.o
	$(COMPILER) $(COMPCFG) -c shader.cpp -o shader.o

	@echo ""
	@echo "<subsystems>"

	@cd renderer
	@make $@
	@cd ..

	@cd material
	@make $@
	@cd ..

	@echo "</subsystems>"
	@echo ""

	@echo "Done!"
	
	$(GET_TIME)
	@echo "[--- END ---]"
	@echo 