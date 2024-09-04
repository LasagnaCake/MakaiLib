CC	:= gcc
CXX	:= g++

COMPILER_CONFIG	:= -m64 -std=gnu++20 -fms-extensions -I../output/include
DEBUGMODE		:= -D_DEBUG_OUTPUT_

all: debug release

debug:
	@echo "Building tests [$@]..."
	@mkdir -p output/$@
	@$(CXX) -o output/$@/test.01.compilation.$@.exe test.01.compilation.cpp $(COMPILER_CONFIG) ../output/lib/libmakai.debug.a $(DEBUGMODE)
	@echo "Done!"
	@echo 

release:
	@echo "Building tests [$@]..."
	@mkdir -p output/$@
	@$(CXX) -o output/$@/test.01.compilation.$@.exe test.01.compilation.cpp $(COMPILER_CONFIG) ../output/lib/libmakai.a -mwindows
	@echo "Done!"
	@echo 
