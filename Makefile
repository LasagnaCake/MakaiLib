define HELP_MESSAGE
Supported targets:
>    test    : Builds enabling most debug features, and with all warnings enabled
>    debug   : Builds enabling debug (console) output, and without optimizations
>    release : Builds enabling optimizations

Supported options:
>    src           = [ value ]        : Specifies the source file             ( DEFAULT: main.cpp  )
>    name          = [ value ]        : Specifies the name of the output file ( DEFAULT: program   )
>    warn          = [ value | none ] : Specifies the warning to enable       ( DEFAULT: none      )
>    debug-profile = [ define ]       : Specifies whether to enable gmon      ( DEFAULT: undefined )
>    keep-o-files  = [ define ]       : Specifies if .o files should be kept  ( DEFAULT: undefined )
endef

src		?= main.cpp
name	?= program

C++ 			:= @g++

COMPILER_CONFIG	:= -fexpensive-optimizations -flto -m64 -std=gnu++20 -fcoroutines -fopenmp -openmp
LINKER_CONFIG	:= -flto -static-libstdc++ -static-libgcc -static -m64

OPTIMIZATIONS	:= -ftree-parallelize-loops=128 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize

INCLUDES		:= -Ilib -Isrc -Ilib\SDL2-2.0.10\include -Ilib\OpenGL -Ilib\OpenGL\GLEW\include -Ilib\cute_headers -Ilib\stb

LIBRARIES		:= lib\SDL2-2.0.10\lib\libSDL2.dll.a lib\SDL2-2.0.10\lib\libSDL2main.a lib\SDL2-2.0.10\lib\libSDL2_mixer.dll.a lib\OpenGL\GLEW\lib\libglew32.dll.a -lopengl32 -lgomp -lpowrprof -lwinmm

# TODO: Clean up this mess

ifdef debug-profile
GMON_OUT := -lgmon
endif

ifndef keep-o-files
MAKE_CLEAN := @make clean CLEAN_TARGET=obj/$@
endif

CLEAN_TARGET ?= obj

ifndef warn
WARNINGS := -w
else
WARNINGS := -W$(warn)
endif

.PHONY: clean debug release all

export HELP_MESSAGE

help:
	@echo "$$HELP_MESSAGE"


test: build\$(src)
	@mkdir -p obj\test
	
	@echo "[0/2] compiling [TEST]..."
	$(C++) $(COMPILER_CONFIG) -Wall -pg -Og -g -fsanitize=leak -fno-omit-frame-pointer -D_DEBUG_OUTPUT_ $(INCLUDES) -c build\$(src) -o obj/test/$(name).o
	
	@echo "[1/2] linking libraries..."
	$(C++) -o res/$(name)TEST.exe obj/test/$(name).o  $(LINKER_CONFIG) -pg $(LIBRARIES)
	
	@echo "[2/2] Done!"
	$(MAKE_CLEAN)


debug: build\$(src)
	@mkdir -p obj\debug
	
	@echo "[0/2] compiling [DEBUG]..."
	$(C++) $(COMPILER_CONFIG) $(WARNINGS) -pg -Og -g -fsanitize=leak -fno-omit-frame-pointer -D_DEBUG_OUTPUT_ $(INCLUDES) -c build\$(src) -o obj/debug/$(name).o
	
	@echo "[1/2] linking libraries..."
	$(C++) -o res/$(name)DEBUG.exe obj/debug/$(name).o  $(LINKER_CONFIG) -pg $(GMON_OUT) $(LIBRARIES)
	
	@echo "[2/2] Done!"
	$(MAKE_CLEAN)


release: build\$(src)
	@mkdir -p obj\release
	
	@echo "[0/2] compiling [RELEASE]..."
	$(C++) $(COMPILER_CONFIG) $(WARNINGS) -lwinpthreads $(OPTIMIZATIONS) $(INCLUDES) -c build\$(src) -o obj/release/$(name).o
	
	@echo "[1/2] linking libraries..."
	$(C++) -o res/$(name).exe obj/release/$(name).o  $(LINKER_CONFIG) -O1  $(LIBRARIES) -mwindows
	
	@echo "[2/2] Done!"
	$(MAKE_CLEAN)

	
clean:
	@echo "Cleaning up..."
	@rm -rf $(CLEAN_TARGET)
	@echo "Done!"