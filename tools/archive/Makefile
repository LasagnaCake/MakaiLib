CC 	?= gcc
CXX ?= g++

ROOT	:= ../..

PACKPROGNAME	:= arcpack
UNPACKPROGNAME	:= arcunpack
KEYGENPROGNAME	:= arcgen

COMPILER_CONFIG	:= -fexpensive-optimizations -m64 -std=gnu++20 -fcoroutines -fms-extensions -DENABLE_DEBUG_OUTPUT_ -DNDEBUG
LINKER_CONFIG	:= -static-libstdc++ -static-libgcc -static -m64 -fms-extensions

OPTIMIZATIONS	:= -fopenmp -openmp -ftree-parallelize-loops=128 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize

WINGARBAGE	:= -lole32 -loleaut32 -limm32 -lwinmm -lversion -lpowrprof -lcomdlg32 -lsetupapi -lgdi32


INCLUDES	:= -I$(ROOT)/src -I$(ROOT)/lib/jsoncpp-3.11.2/include -I$(ROOT)/lib/cryptopp/include -I$(ROOT)/lib/cppcodec-0.2
LIBRARIES	:= $(WINGARBAGE) $(ROOT)/lib/cryptopp/lib/libcryptopp.a -lopengl32 -lgomp

DEBUGCOMPCFG	:= $(COMPILER_CONFIG) -Wall -Wpedantic -pg -Og -ggdb3 -fno-omit-frame-pointer
DEBUGLINKCFG	:= $(LINKER_CONFIG) -pg -Og $(LIBRARIES)

COMPCFG	:= $(COMPILER_CONFIG) -O3 $(OPTIMIZATIONS) -D_DEBUG_OUTPUT_
LINKCFG	:= $(LINKER_CONFIG) -O3 $(LIBRARIES)

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
	$(COMPILER) $(DEBUGCOMPCFG) -c $(PACKPROGNAME).cpp -o $(PACKPROGNAME)_$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c $(UNPACKPROGNAME).cpp -o $(UNPACKPROGNAME)_$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c $(KEYGENPROGNAME).cpp -o $(KEYGENPROGNAME)_$@.o
	
	@echo "[2/2] linking libraries..."
	$(LINKER) -o $(PACKPROGNAME)_$@.exe $(PACKPROGNAME)_$@.o $(DEBUGLINKCFG)
	$(LINKER) -o $(UNPACKPROGNAME)_$@.exe $(UNPACKPROGNAME)_$@.o $(DEBUGLINKCFG)
	$(LINKER) -o $(KEYGENPROGNAME)_$@.exe $(KEYGENPROGNAME)_$@.o $(DEBUGLINKCFG)
	
	@echo "Done!"
	
	@echo "Cleaning up..."
	@rm -rf $(PACKPROGNAME)_$@.o
	@rm -rf $(UNPACKPROGNAME)_$@.o
	@rm -rf $(KEYGENPROGNAME)_$@.o
	@echo "Done!"
	
	$(GET_TIME)
	
	@echo "[--- END ---]"
	@echo 

release:
	@echo ""
	@echo "[--- START ---]"
	
	$(GET_TIME)
	
	@echo "[1/2] compiling [$@]..."
	$(COMPILER) $(COMPCFG) -c $(PACKPROGNAME).cpp -o $(PACKPROGNAME).o
	$(COMPILER) $(COMPCFG) -c $(UNPACKPROGNAME).cpp -o $(UNPACKPROGNAME).o
	$(COMPILER) $(COMPCFG) -c $(KEYGENPROGNAME).cpp -o $(KEYGENPROGNAME).o
	
	@echo "[2/2] linking libraries..."
	$(LINKER) -o $(PACKPROGNAME).exe $(PACKPROGNAME).o $(LINKCFG)
	$(LINKER) -o $(UNPACKPROGNAME).exe $(UNPACKPROGNAME).o $(LINKCFG)
	$(LINKER) -o $(KEYGENPROGNAME).exe $(KEYGENPROGNAME).o $(LINKCFG)
	
	@echo "Cleaning up..."
	@rm -rf $(PACKPROGNAME).o
	@rm -rf $(UNPACKPROGNAME).o
	@rm -rf $(KEYGENPROGNAME).o
	@echo "Done!"
	
	$(GET_TIME)
	
	@echo "[--- END ---]"
	@echo 