CC 	?= gcc
CXX ?= g++

PACKPROGNAME	:= mkaPack
UNPACKPROGNAME	:= mkaUnpack

COMPILER_CONFIG	:= -fexpensive-optimizations -flto -m64 -std=gnu++20 -fcoroutines -fms-extensions
LINKER_CONFIG	:= -flto -static-libstdc++ -static-libgcc -static -m64 -fms-extensions
OPTIMIZATIONS	+= -fopenmp -openmp -ftree-parallelize-loops=128 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize

INCLUDES	:=
LIBRARIES	:=

COMPILER	:= @$(CXX) $(COMPILER_CONFIG) -Wall -Wpedantic -pg -Og -ggdb3 -fno-omit-frame-pointer -D_DEBUG_OUTPUT_ $(INCLUDES)
LINKER		:= @$(CXX) $(LINKER_CONFIG) -pg -Og $(LIBRARIES) $(WINGARBAGE)

GUI_MODE	?= -mwindows

WINGARBAGE		:= -lole32 -loleaut32 -limm32 -lwinmm -lversion -lpowrprof -lcomdlg32 -lsetupapi -lgdi32

debug:
	@echo "[1/2] compiling [$@]..."
	$(COMPILER) $(PACKPROGNAME)_$@.cpp -o $(PACKPROGNAME)_$@.o
	$(COMPILER) $(UNPACKPROGNAME)_$@.cpp -o $(UNPACKPROGNAME)_$@.o
	
	@echo "[2/2] linking libraries..."
	$(LINKER) -o $(PACKPROGNAME)_$@.exe $(PACKPROGNAME)_$@.o 
	$(LINKER) -o $(UNPACKPROGNAME)_$@.exe $(UNPACKPROGNAME)_$@.o

release:
	@echo "[1/2] compiling [$@]..."
	$(COMPILER) $(PACKPROGNAME).cpp -o $(PACKPROGNAME).o
	$(COMPILER) $(UNPACKPROGNAME).cpp -o $(UNPACKPROGNAME).o
	
	@echo "[2/2] linking libraries..."
	$(LINKER) -o $(PACKPROGNAME).exe $(PACKPROGNAME).o 
	$(LINKER) -o $(UNPACKPROGNAME).exe $(UNPACKPROGNAME).o

	