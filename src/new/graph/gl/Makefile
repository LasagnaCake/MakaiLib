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

lower = $(shell echo $(1) | tr A-Z a-z)
upper = $(shell echo $(1) | tr a-z A-Z)

ifndef gl-loader
gl-loader := glad
endif

override prefix := $(strip $(prefix)).gl

OPTIMIZATIONS	:= $(USE_OPENMP) -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize $(LEAN)

INCLUDES := -I$(ROOT)/lib/SDL2-2.0.10/include -I$(ROOT)/lib/OpenGL -I$(ROOT)/lib/OpenGL/$(call upper,$(gl-loader))/include -I$(ROOT)/lib/stb

DEBUGMODE := -DMAKAILIB_DEBUG -DCTL_DEBUG -DNDEBUG
DEBUGCOMPCFG	:= $(COMPILER_CONFIG) -Wall -Wpedantic -Og -ggdb3 -fno-omit-frame-pointer $(DEBUGMODE)
DEBUGLINKCFG	:= $(LINKER_CONFIG) -Og $(LIBRARIES)

ifdef debug-release
DEBUGRELEASE := $(DEBUGMODE)
export debug-release
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
	$(COMPILER) $(DEBUGCOMPCFG) -c core.cpp -o $(prefix).core.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c global.cpp -o $(prefix).global.$@.o
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
	$(COMPILER) $(COMPCFG) -c core.cpp -o $(prefix).core.$@.o
	$(COMPILER) $(COMPCFG) -c global.cpp -o $(prefix).global.$@.o
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
