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
	
	@echo "[1/3] making [$@:pack]..."
	@make pack-$@ --no-print-directory
	@echo "[2/3] making [$@:unpack]..."
	@make unpack-$@ --no-print-directory
	@echo "[3/3] making [$@:keygen]..."
	@make keygen-$@ --no-print-directory
	@echo "Done!"
	
	$(GET_TIME)
	
	@echo "[--- END ---]"
	@echo 

release:
	@echo ""
	@echo "[--- START ---]"
	
	$(GET_TIME)
	
	@echo "[1/3] making [$@:pack]..."
	@make pack-$@ --no-print-directory
	@echo "[2/3] making [$@:unpack]..."
	@make unpack-$@ --no-print-directory
	@echo "[3/3] making [$@:keygen]..."
	@make keygen-$@ --no-print-directory
	@echo "Done!"
	
	$(GET_TIME)
	
	@echo "[--- END ---]"
	@echo 

pack:
	@echo ""
	@echo "[--- START ---]"
	
	$(GET_TIME)
	
	@echo "[1/2] making [$@:debug]..."
	@make $@-debug --no-print-directory
	@echo "[2/2] making [$@:release]..."
	@make $@-release --no-print-directory
	@echo "Done!"
	
	$(GET_TIME)
	
	@echo "[--- END ---]"
	@echo 

unpack:
	@echo ""
	@echo "[--- START ---]"
	
	$(GET_TIME)
	
	@echo "[1/2] making [$@:debug]..."
	@make $@-debug --no-print-directory
	@echo "[2/2] making [$@:release]..."
	@make $@-release --no-print-directory
	@echo "Done!"
	
	$(GET_TIME)
	
	@echo "[--- END ---]"
	@echo 

keygen:
	@echo ""
	@echo "[--- START ---]"
	
	$(GET_TIME)
	
	@echo "[1/2] making [$@:debug]..."
	@make $@-debug --no-print-directory
	@echo "[2/2] making [$@:release]..."
	@make $@-release --no-print-directory
	@echo "Done!"
	
	$(GET_TIME)
	
	@echo "[--- END ---]"
	@echo 

pack-debug:
	$(COMPILER) $(DEBUGCOMPCFG) -c $(PACKPROGNAME).cpp -o $(PACKPROGNAME)_debug.o
	$(LINKER) -o $(PACKPROGNAME)_debug.exe $(PACKPROGNAME)_debug.o $(DEBUGLINKCFG)
	@rm -rf $(PACKPROGNAME)_debug.o
	@echo 

unpack-debug:
	$(COMPILER) $(DEBUGCOMPCFG) -c $(UNPACKPROGNAME).cpp -o $(UNPACKPROGNAME)_debug.o
	$(LINKER) -o $(UNPACKPROGNAME)_debug.exe $(UNPACKPROGNAME)_debug.o $(DEBUGLINKCFG)
	@rm -rf $(UNPACKPROGNAME)_debug.o
	@echo 

keygen-debug:
	$(COMPILER) $(DEBUGCOMPCFG) -c $(KEYGENPROGNAME).cpp -o $(KEYGENPROGNAME)_debug.o
	$(LINKER) -o $(KEYGENPROGNAME)_debug.exe $(KEYGENPROGNAME)_debug.o $(DEBUGLINKCFG)
	@rm -rf $(KEYGENPROGNAME)_debug.o
	@echo 

pack-release:
	$(COMPILER) $(COMPCFG) -c $(PACKPROGNAME).cpp -o $(PACKPROGNAME).o
	$(LINKER) -o $(PACKPROGNAME).exe $(PACKPROGNAME).o $(LINKCFG)
	@rm -rf $(PACKPROGNAME).o
	@echo 

unpack-release:
	$(COMPILER) $(COMPCFG) -c $(UNPACKPROGNAME).cpp -o $(UNPACKPROGNAME).o
	$(LINKER) -o $(UNPACKPROGNAME).exe $(UNPACKPROGNAME).o $(LINKCFG)
	@rm -rf $(UNPACKPROGNAME).o
	@echo 

keygen-release:
	$(COMPILER) $(COMPCFG) -c $(KEYGENPROGNAME).cpp -o $(KEYGENPROGNAME).o
	$(LINKER) -o $(KEYGENPROGNAME).exe $(KEYGENPROGNAME).o $(LINKCFG)
	@rm -rf $(KEYGENPROGNAME).o
	@echo 