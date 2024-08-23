CC 	?= gcc
CXX ?= g++

COMPILER_CONFIG	:= -fexpensive-optimizations -m64 -std=gnu++20 -fconcepts -fcoroutines -fms-extensions -static -s
LINKER_CONFIG	:= -static-libstdc++ -static-libgcc -static -m64 -fms-extensions -s

DEBUGCOMPCFG	:= $(COMPILER_CONFIG) -Wall -Wpedantic -pg -Og -ggdb3 -fno-omit-frame-pointer -D_DEBUG_OUTPUT_ -DENABLE_DEBUG_OUTPUT_ -DNDEBUG
DEBUGLINKCFG	:= $(LINKER_CONFIG) -pg -Og $(LIBRARIES)

COMPCFG	:= $(COMPILER_CONFIG) -O3 $(OPTIMIZATIONS)
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
	$(COMPILER) $(DEBUGCOMPCFG) -c json.cpp -o json_debug.o
	$(COMPILER) $(DEBUGCOMPCFG) -c slf.cpp -o slf_debug.o
	$(COMPILER) $(DEBUGCOMPCFG) -c get.cpp -o get_debug.o

	@echo "Done!"
	
	$(GET_TIME)
	@echo "[--- END ---]"
	@echo 


release:
	@echo ""
	@echo "[--- START ---]"
	$(GET_TIME)
	
	@echo "[1/2] compiling [$@]..."
	$(COMPILER) $(COMPCFG) -c json.cpp -o json.o
	$(COMPILER) $(COMPCFG) -c slf.cpp -o slf.o
	$(COMPILER) $(COMPCFG) -c get.cpp -o get.o

	@echo "Done!"
	
	$(GET_TIME)
	@echo "[--- END ---]"
	@echo 