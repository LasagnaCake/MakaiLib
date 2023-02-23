target		?= main.cpp
name		?= program

COMPILE_REQ	:= -flto -m64 -std=gnu++20 -fcoroutines -fopenmp -openmp
CONFIG_REQ	:= -flto -static-libstdc++ -static-libgcc -static -m64

INCLUDES	:= -Ilib -Isrc -Ilib\SDL2-2.0.10\include -Ilib\OpenGL -Ilib\OpenGL\GLEW\include -Ilib\cute_headers -Ilib\stb

LIBRARIES	:= lib\SDL2-2.0.10\lib\libSDL2.dll.a lib\SDL2-2.0.10\lib\libSDL2main.a lib\SDL2-2.0.10\lib\libSDL2_mixer.dll.a lib\OpenGL\GLEW\lib\libglew32.dll.a -lopengl32 -lgomp -lpowrprof -lwinmm

# TODO: Clean up this mess

.PHONY: clean debug release all

all: debug release

debug: build\$(target)
	@mkdir -p obj\debug

	@g++.exe -fexpensive-optimizations -Wall $(COMPILE_REQ) -pg -Og -g -fsanitize=leak -fno-omit-frame-pointer -D_DEBUG_OUTPUT_ $(INCLUDES) -c build\$(target) -o $(name).o
	
	@g++.exe -o res/$(name)DEBUG.exe $(name).o  $(CONFIG_REQ) -pg -lgmon $(LIBRARIES)
	
	@make clean


release: build\$(target)
	@mkdir -p obj\release

	@g++.exe -fexpensive-optimizations $(COMPILE_REQ) -w -lwinpthreads -ftree-parallelize-loops=128 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize $(INCLUDES) -c build\$(target) -o $(name).o
	
	@g++.exe -o res/$(name).exe $(name).o  $(CONFIG_REQ) -O1  $(LIBRARIES) -mwindows
	
	@make clean

	
clean:
	@rm -rf obj