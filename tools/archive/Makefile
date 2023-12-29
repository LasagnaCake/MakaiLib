CC 	?= gcc
CXX ?= g++

PACKPROGNAME	:= arcpack
UNPACKPROGNAME	:= arcunpack

COMPILER_CONFIG	:= -fexpensive-optimizations -flto -m64 -std=gnu++20 -fcoroutines -fms-extensions
LINKER_CONFIG	:= -flto -static-libstdc++ -static-libgcc -static -m64 -fms-extensions

OPTIMIZATIONS	:= -fopenmp -openmp -ftree-parallelize-loops=128 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize

WINGARBAGE	:= -lole32 -loleaut32 -limm32 -lwinmm -lversion -lpowrprof -lcomdlg32 -lsetupapi -lgdi32

INCLUDES	:= -I../../src/collection -I../../lib/jsoncpp-3.11.2/include
LIBRARIES	:= $(WINGARBAGE)

DEBUGCOMPCFG	:= $(COMPILER_CONFIG) -Wall -Wpedantic -pg -Og -ggdb3 -fno-omit-frame-pointer -D_DEBUG_OUTPUT_
DEBUGLINKCFG	:= $(LINKER_CONFIG) -pg -Og

COMPCFG	:= -O3 $(COMPILER_CONFIG) $(OPTIMIZATIONS)
LINKCFG	:= -O3 $(LINKER_CONFIG) $(GUI_MODE)

COMPILER	:= @$(CXX) $(INCLUDES)
LINKER		:= @$(CXX) $(LIBRARIES)

GUI_MODE	?= -mwindows

.PHONY: debug release all
.ONESHELL:
.SHELLFLAGS = -ec

debug:
	@echo ""
	@echo "[--- START ---]"
	
	$(GET_TIME)
	
	@echo "[1/2] compiling [$@]..."
	$(COMPILER) $(DEBUGCOMPCFG) $(PACKPROGNAME).cpp -o $(PACKPROGNAME)_$@.o
	$(COMPILER) $(DEBUGCOMPCFG) $(UNPACKPROGNAME).cpp -o $(UNPACKPROGNAME)_$@.o
	
	@echo "[2/2] linking libraries..."
	$(LINKER) -o $(PACKPROGNAME)_$@.exe $(PACKPROGNAME)_$@.o $(DEBUGLINKCFG)
	$(LINKER) -o $(UNPACKPROGNAME)_$@.exe $(UNPACKPROGNAME)_$@.o $(DEBUGLINKCFG)
	
	@echo "Done!"
	
	@echo "Cleaning up..."
	@rm -rf $(PACKPROGNAME)_$@.o
	@rm -rf $(UNPACKPROGNAME)_$@.o
	@echo "Done!"
	
	$(GET_TIME)
	
	@echo "[--- END ---]"
	@echo 

release:
	@echo ""
	@echo "[--- START ---]"
	
	$(GET_TIME)
	
	@echo "[1/2] compiling [$@]..."
	$(COMPILER) $(COMPCFG) $(PACKPROGNAME).cpp -o $(PACKPROGNAME).o
	$(COMPILER) $(COMPCFG) $(UNPACKPROGNAME).cpp -o $(UNPACKPROGNAME).o
	
	@echo "[2/2] linking libraries..."
	$(LINKER) -o $(PACKPROGNAME).exe $(PACKPROGNAME).o $(LINKCFG)
	$(LINKER) -o $(UNPACKPROGNAME).exe $(UNPACKPROGNAME).o $(LINKCFG)
	
	@echo "Cleaning up..."
	@rm -rf $(PACKPROGNAME).o
	@rm -rf $(UNPACKPROGNAME).o
	@echo "Done!"
	
	$(GET_TIME)
	
	@echo "[--- END ---]"
	@echo 

all: debug release