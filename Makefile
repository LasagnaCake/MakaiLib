target	?= main.cpp
name	?= program

# TODO: Clean up this mess

all: debug release

debug: build\$(target)
	mkdir -p obj\debug

	g++.exe -fexpensive-optimizations -flto -Wall -DDEBUG -m64 -std=gnu++20 -fcoroutines -fopenmp -openmp -pg -Og -g -fsanitize=leak -fno-omit-frame-pointer -D_DEBUG_OUTPUT_ -Ilib -Isrc -Ilib\SDL2-2.0.10\include -Ilib\OpenGL -Ilib\OpenGL\GLEW\include -Ilib\cute_headers -Ilib\stb -c build\$(target) -o obj\debug\$(name).o
	
	g++.exe -Llib -Llib\SDL2-2.0.10\include -Llib\OpenGL -Llib\OpenGL\GLEW\include -Llib\cute_headers -Llib\stb -o res/$(name)DEBUG.exe obj\debug\$(name).o  -flto -static-libstdc++ -static-libgcc -static -m64 -pg -lgmon  lib\SDL2-2.0.10\lib\libSDL2.dll.a lib\SDL2-2.0.10\lib\libSDL2main.a lib\SDL2-2.0.10\lib\libSDL2_mixer.dll.a lib\OpenGL\GLEW\lib\libglew32.dll.a -lopengl32 -lgomp -lopengl32 -lpowrprof -lwinmm


release: build\$(target)
	mkdir -p obj\release

	g++.exe -fexpensive-optimizations -flto -Wall -m64 -std=gnu++20 -fcoroutines -fopenmp -openmp -lwinpthreads -ftree-parallelize-loops=128 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize -Ilib -Isrc -Ilib\SDL2-2.0.10\include -Ilib\OpenGL -Ilib\OpenGL\GLEW\include -Ilib\cute_headers -Ilib\stb -c build\$(target) -o obj\release\$(name).o
	
	g++.exe -Llib -Llib\SDL2-2.0.10\include -Llib\OpenGL -Llib\OpenGL\GLEW\include -Llib\cute_headers -Llib\stb -o res/$(name).exe obj\release\$(name).o  -flto -static-libstdc++ -static-libgcc -static -m64 -O1  lib\SDL2-2.0.10\lib\libSDL2.dll.a lib\SDL2-2.0.10\lib\libSDL2main.a lib\SDL2-2.0.10\lib\libSDL2_mixer.dll.a lib\OpenGL\GLEW\lib\libglew32.dll.a -lopengl32 -lgomp -lopengl32 -lpowrprof -lwinmm -mwindows
