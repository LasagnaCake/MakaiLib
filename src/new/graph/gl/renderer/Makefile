CC 	?= gcc
CXX ?= g++

LEAN := -static -s

ROOT	:= ../../../../..

COMPILER_CONFIG	:= -fexpensive-optimizations -m64 -std=gnu++20 -fconcepts -fcoroutines -fms-extensions
LINKER_CONFIG	:= -static-libstdc++ -static-libgcc -m64 -fms-extensions

OPTIMIZATIONS	:= -fopenmp -openmp -ftree-parallelize-loops=128 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize $(LEAN)

INCLUDES := -I$(ROOT)/lib/SDL2-2.0.10/include -I$(ROOT)/lib/OpenGL

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
	$(COMPILER) $(DEBUGCOMPCFG) -c mode.cpp -o mode_debug.o
	$(COMPILER) $(DEBUGCOMPCFG) -c core.cpp -o core_debug.o
	$(COMPILER) $(DEBUGCOMPCFG) -c drawable.cpp -o drawable_debug.o
	$(COMPILER) $(DEBUGCOMPCFG) -c reference.cpp -o reference_debug.o
	$(COMPILER) $(DEBUGCOMPCFG) -c renderable.cpp -o renderable_debug.o
	$(COMPILER) $(DEBUGCOMPCFG) -c bar.cpp -o bar_debug.o
	$(COMPILER) $(DEBUGCOMPCFG) -c text.cpp -o text_debug.o
	$(COMPILER) $(DEBUGCOMPCFG) -c scene.cpp -o scene_debug.o
	$(COMPILER) $(DEBUGCOMPCFG) -c framebuffer.cpp -o framebuffer_debug.o

	@echo "Done!"
	
	$(GET_TIME)
	@echo "[--- END ---]"
	@echo 


release:
	@echo ""
	@echo "[--- START ---]"
	$(GET_TIME)
	
	@echo "[1/2] compiling [$@]..."
	$(COMPILER) $(COMPCFG) -c mode.cpp -o mode.o
	$(COMPILER) $(COMPCFG) -c core.cpp -o core.o
	$(COMPILER) $(COMPCFG) -c drawable.cpp -o drawable.o
	$(COMPILER) $(COMPCFG) -c reference.cpp -o reference.o
	$(COMPILER) $(COMPCFG) -c renderable.cpp -o renderable.o
	$(COMPILER) $(COMPCFG) -c bar.cpp -o bar.o
	$(COMPILER) $(COMPCFG) -c text.cpp -o text.o
	$(COMPILER) $(COMPCFG) -c scene.cpp -o scene.o
	$(COMPILER) $(COMPCFG) -c framebuffer.cpp -o framebuffer.o

	@echo "Done!"
	
	$(GET_TIME)
	@echo "[--- END ---]"
	@echo 