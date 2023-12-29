CC 	?= gcc
CXX ?= g++

PACKPROGNAME	:= mkaPack
UNPACKPROGNAME	:= mkaUnpack

COMPILER_CONFIG	:= -fexpensive-optimizations -flto -m64 -std=gnu++20 -fcoroutines -fms-extensions
LINKER_CONFIG	:= -flto -static-libstdc++ -static-libgcc -static -m64 -fms-extensions
OPTIMIZATIONS	+= -fopenmp -openmp -ftree-parallelize-loops=128 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize

WINGARBAGE	:= -lole32 -loleaut32 -limm32 -lwinmm -lversion -lpowrprof -lcomdlg32 -lsetupapi -lgdi32

INCLUDES	:= -I../src/collection -I../lib/jsoncpp-3.11.2/include
LIBRARIES	:= $(WINGARBAGE)

COMPILER	:= @$(CXX) $(COMPILER_CONFIG) -Wall -Wpedantic -pg -Og -ggdb3 -fno-omit-frame-pointer -D_DEBUG_OUTPUT_ $(INCLUDES)
LINKER		:= @$(CXX) $(LINKER_CONFIG) -pg -Og $(LIBRARIES)

GUI_MODE	?= -mwindows

.PHONY: debug release all
.ONESHELL:
.SHELLFLAGS = -ec

debug:
	@echo ""
	@echo "[--- START ---]"
	
	$(GET_TIME)
	
	@echo "[1/2] compiling [$@]..."
	$(COMPILER) $(PACKPROGNAME)_$@.cpp -o $(PACKPROGNAME)_$@.o
	$(COMPILER) $(UNPACKPROGNAME)_$@.cpp -o $(UNPACKPROGNAME)_$@.o
	
	@echo "[2/2] linking libraries..."
	$(LINKER) -o $(PACKPROGNAME)_$@.exe $(PACKPROGNAME)_$@.o 
	$(LINKER) -o $(UNPACKPROGNAME)_$@.exe $(UNPACKPROGNAME)_$@.o
	
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
	$(COMPILER) $(PACKPROGNAME).cpp -o $(PACKPROGNAME).o
	$(COMPILER) $(UNPACKPROGNAME).cpp -o $(UNPACKPROGNAME).o
	
	@echo "[2/2] linking libraries..."
	$(LINKER) -o $(PACKPROGNAME).exe $(PACKPROGNAME).o 
	$(LINKER) -o $(UNPACKPROGNAME).exe $(UNPACKPROGNAME).o
	
	@echo "Cleaning up..."
	@rm -rf $(PACKPROGNAME).o
	@rm -rf $(UNPACKPROGNAME).o
	@echo "Done!"
	
	$(GET_TIME)
	
	@echo "[--- END ---]"
	@echo 

all: debug release