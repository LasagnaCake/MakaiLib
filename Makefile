target	?= main.cpp
name	?= program
warn	?= no

COMPILER_CONFIG	:= -fexpensive-optimizations -flto -m64 -std=gnu++20 -fcoroutines -fopenmp -openmp
LINKER_CONFIG	:= -flto -static-libstdc++ -static-libgcc -static -m64

OPTIMIZATIONS	:= -ftree-parallelize-loops=128 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize

INCLUDES	:= -Ilib -Isrc -Ilib\SDL2-2.0.10\include -Ilib\OpenGL -Ilib\OpenGL\GLEW\include -Ilib\cute_headers -Ilib\stb

LIBRARIES	:= lib\SDL2-2.0.10\lib\libSDL2.dll.a lib\SDL2-2.0.10\lib\libSDL2main.a lib\SDL2-2.0.10\lib\libSDL2_mixer.dll.a lib\OpenGL\GLEW\lib\libglew32.dll.a -lopengl32 -lgomp -lpowrprof -lwinmm

# TODO: Clean up this mess

ifeq ($(debug-gmon-profile), yes)
GMON_OUT := -lgmon
endif

ifneq ($(keep-object-files), yes)
MAKE_CLEAN := @make clean
endif

ifeq ($(warn), no)
WARNINGS := -w
else
WARNINGS := -W$(warn)
endif

.PHONY: clean debug release all

all: debug release

debug: build\$(target)
	@mkdir -p obj\debug
	
	@echo "[0/2] compiling [DEBUG]..."
	@g++.exe $(COMPILER_CONFIG) $(WARNINGS) -pg -Og -g -fsanitize=leak -fno-omit-frame-pointer -D_DEBUG_OUTPUT_ $(INCLUDES) -c build\$(target) -o $(name).o
	
	@echo "[1/2] linking libraries..."
	@g++.exe -o res/$(name)DEBUG.exe $(name).o  $(LINKER_CONFIG) -pg $(GMON_OUT) $(LIBRARIES)
	
	@echo "[2/2] Done!"
	$(MAKE_CLEAN)


release: build\$(target)
	@mkdir -p obj\release
	
	@echo "[0/2] compiling [RELEASE]..."
	@g++.exe $(COMPILER_CONFIG) $(WARNINGS) -lwinpthreads $(OPTIMIZATIONS) $(INCLUDES) -c build\$(target) -o $(name).o
	
	@echo "[1/2] linking libraries..."
	@g++.exe -o res/$(name).exe $(name).o  $(LINKER_CONFIG) -O1  $(LIBRARIES) -mwindows
	
	@echo "[2/2] Done!"
	$(MAKE_CLEAN)

	
clean:
	@echo "Cleaning up..."
	@rm -rf obj
	@echo "Done!"