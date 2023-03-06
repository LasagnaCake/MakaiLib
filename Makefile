define HELP_MESSAGE
Supported targets:
>    debug   : Builds enabling debug options, and with all warnings enabled (+ pedantic)
>    test    : Builds enabling debug options, and without optimizations
>    release : Builds enabling optimizations
>    both    : Builds both debug and release

Supported options:
>    src           = [ value ]        : Specifies the source file             ( DEFAULT: main.cpp )
>    name          = [ value ]        : Specifies the name of the output file ( DEFAULT: program  )
>    warn          = [ value | none ] : Specifies the warning to enable       ( DEFAULT: none     )
>    use-openmp    = [ 1 | 0 ]        : Specifies whether ro enable OpenMP    ( DEFAULT: none     )
>    use-openmp    = [ 0 - 3 ]        : Specifies the optimization level      ( DEFAULT: 3        )
>    debug-profile = [ 1 | 0 ]        : Specifies whether to enable gmon      ( DEFAULT: 0        )
>    keep-o-files  = [ 1 | 0 ]        : Specifies if .o files should be kept  ( DEFAULT: 0        )
endef

src				?= main.cpp
name			?= program
debug-profile	?= 0
keep-o-files	?= 0
use-openmp		?= 0

CC 	?= gcc
CXX ?= g++

COMPILER_CONFIG	:= -fexpensive-optimizations -flto -m64 -std=gnu++20 -fcoroutines
LINKER_CONFIG	:= -flto -static-libstdc++ -static-libgcc -static -m64

ifeq ($(use-openmp), 1)
OPTIMIZATIONS	:= -fopenmp -openmp -ftree-parallelize-loops=128 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize
OPENMP_ENABLED	:= -D_USE_OPENMP_
optimize-level	?= 1
else
optimize-level	?= 3
endif

INCLUDES		:= -Ilib -Isrc -Ilib\SDL2-2.0.10\include -Ilib\OpenGL -Ilib\OpenGL\GLEW\include -Ilib\cute_headers -Ilib\stb -Ilib\libzipp-v6.0-1.9.2\include

LIBRARIES		:= lib\SDL2-2.0.10\lib\libSDL2.dll.a lib\SDL2-2.0.10\lib\libSDL2main.a lib\SDL2-2.0.10\lib\libSDL2_mixer.dll.a lib\OpenGL\GLEW\lib\libglew32.dll.a -lopengl32 -lgomp -lpowrprof -lwinmm lib\libzipp-v6.0-1.9.2\lib\libzippp.a

# TODO: Clean up this mess

ifneq ($(debug-profile), 0)
GMON_OUT := -lgmon
endif

ifeq ($(keep-o-files), 0)
MAKE_CLEAN := @make clean CLEAN_TARGET=obj/$@
endif

CLEAN_TARGET ?= obj

ifndef warn
WARNINGS := -w
else
WARNINGS := -W$(warn)
endif

GUI_MODE ?= -mwindows

.PHONY: clean debug release all

export HELP_MESSAGE

help:
	@echo "$$HELP_MESSAGE"


debug: build\$(src)
	@mkdir -p obj\$@
	
	@echo "[0/2] compiling [$@]..."
	@$(CXX) $(COMPILER_CONFIG) -Wall -Wpedantic -pg -Og -g -fsanitize=leak -fno-omit-frame-pointer -D_DEBUG_OUTPUT_ $(INCLUDES) -c build\$(src) -o obj/$@/$(name).o
	
	@echo "[1/2] linking libraries..."
	@$(CXX) -o res/$(name)_$@.exe obj/$@/$(name).o  $(LINKER_CONFIG) -pg -Og $(LIBRARIES)
	
	@echo "[2/2] Done!"
	$(MAKE_CLEAN)


test: build\$(src)
	@mkdir -p obj\$@
	
	@echo "[0/2] compiling [$@]..."
	@$(CXX) $(COMPILER_CONFIG) $(WARNINGS) -pg -Og -g -fsanitize=leak -fno-omit-frame-pointer -D_DEBUG_OUTPUT_ $(INCLUDES) -c build\$(src) -o obj/$@/$(name).o
	
	@echo "[1/2] linking libraries..."
	@$(CXX) -o res/$(name)_$@.exe obj/$@/$(name).o  $(LINKER_CONFIG) -pg -Og $(GMON_OUT) $(LIBRARIES)
	
	@echo "[2/2] Done!"
	$(MAKE_CLEAN)


release: build\$(src)
	@mkdir -p obj\$@
	
	@echo "[0/2] compiling [$@]..."
	@$(CXX) $(COMPILER_CONFIG) $(WARNINGS) -lwinpthreads $(OPTIMIZATIONS) $(OPENMP_ENABLED) $(INCLUDES) -c build\$(src) -o obj/$@/$(name).o
	
	@echo "[1/2] linking libraries..."
	@$(CXX) -o res/$(name).exe obj/$@/$(name).o  $(LINKER_CONFIG) -O$(optimize-level)  $(LIBRARIES) $(GUI_MODE)
	
	@echo "[2/2] Done!"
	$(MAKE_CLEAN)

both: debug release
	
clean:
	@echo "Cleaning up..."
	@rm -rf $(CLEAN_TARGET)
	@echo "Done!"