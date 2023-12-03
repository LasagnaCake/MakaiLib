define HELP_MESSAGE
Supported targets:
>    debug   : Builds enabling debug options, and with all warnings enabled (+ pedantic)
>    release : Builds enabling optimizations
>    demo    : Builds enabling optimizations, and with the _DEMO_BUILD_ macro defined
>    test    : Builds both debug and release
>    both    : Builds both release and demo
>    all     : Builds all options (debug, demo and release)

Supported options:
>    src           = [ value ]        : Spec. the source file                 ( DEFAULT: main.cpp )
>    name          = [ value ]        : Spec. the name of the output file     ( DEFAULT: program  )
>    warn          = [ value | none ] : Spec. the warning to enable           ( DEFAULT: none     )
>    use-openmp    = [ 1 | 0 ]        : Spec. whether ro enable OpenMP        ( DEFAULT: 0        )
>    optimize-lvl  = [ 0 - 3 | g ]    : Spec. the optimization level          ( DEFAULT: 2        )
>    debug-profile = [ 1 | 0 ]        : Spec. whether to enable gmon          ( DEFAULT: 0        )
>    keep-o-files  = [ 1 | 0 ]        : Spec. if .o files should be kept      ( DEFAULT: 0        )
>    macro         = [ value | none ] : Spec. a macro to be defined           ( DEFAULT: none     )
>    meth          = [ 1 : 0 ]        : Spec. whether to enable fast math     ( DEFAULT: 0        )
>    sath          = [ 1 : 0 ]        : Spec. whether to enable safe math     ( DEFAULT: 0        )
>    debug-release = [ 1 : 0 ]        : Spec. whether to enable -g on release ( DEFAULT: 0        )
>    rcscript      = [ value | none ] : Spec. the location of a .rc file      ( DEFAULT: 0        )

Pack-exclusive options:
>    extra-files   = [ value | none ] : Spec. extra files to pack in zip file ( DEFAULT: none     )

NOTES:
(Safe, in this case, means 'IEEE compliant'.)
>    meth   : Enables FAST, but IMPRECISE and UNSAFE floating point operations.
>    sath   : Enables SAFE math. If this one is enabled, METH is disabled.
>    rcfile : Must be located within the 'build/' folder.
>    name   : For targers "debug" and "demo", "_debug" and "_demo" gets appended at the end of the file, respectively.

Pack:
>    name   : Both the name used for the output zip file, and the name of the .exe, minus the compilation target name.
endef

define GET_TIME
@printf "\nTime: "
@date +”%H:%M:%S”
@echo ""
endef

src				?= main.cpp
name			?= program
debug-profile	?= 0
keep-o-files	?= 0
use-openmp		?= 0
meth			?= 0
sath			?= 0
debug-release	?= 0

CC 	?= gcc
CXX ?= g++

COMPILER_CONFIG	:= -fexpensive-optimizations -flto -m64 -std=gnu++20 -fcoroutines -fms-extensions
LINKER_CONFIG	:= -flto -static-libstdc++ -static-libgcc -static -m64 -fms-extensions

ifeq ($(sath), 1)
meth				:= 0
SAFE_MATH			:= -frounding-math -fsignaling-nans
endif

ifeq ($(meth), 1)
MATH_OPTIMIZATIONS	:= -ffast-math -fsingle-precision-constant
endif

OPTIMIZATIONS	:= $(MATH_OPTIMIZATIONS)

ifeq ($(use-openmp), 1)
OPTIMIZATIONS	+= -fopenmp -openmp -ftree-parallelize-loops=128 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize
OPENMP_ENABLED	:= -D_USE_OPENMP_
optimize-lvl	:= g
else
optimize-lvl	?= 2
endif

INCLUDES		:= -Ilib -Isrc -Ilib\SDL2-2.0.10\include -Ilib\OpenGL -Ilib\OpenGL\GLEW\include -Ilib\cute_headers -Ilib\stb -Ilib\jsoncpp-3.11.2\include -Ilib\cppcodec-0.2

LIBRARIES		:= lib\SDL2-2.0.10\lib\libSDL2.dll.a lib\SDL2-2.0.10\lib\libSDL2main.a lib\SDL2-2.0.10\lib\libSDL2_mixer.dll.a lib\OpenGL\GLEW\lib\libglew32.dll.a -lopengl32 -lgomp -lpowrprof -lwinmm -lcomdlg32

# This doesn't work. Oh well.
# SANITIZER_OPTIONS := -fsanitize=leak -fsanitize=memory

# TODO: Clean up this mess

ifneq ($(debug-profile), 0)
GMON_OUT := -lgmon
endif

ifeq ($(keep-o-files), 0)
MAKE_CLEAN := @make clean CLEAN_TARGET=obj/$@ --no-print-directory -j1
endif

ifneq ($(debug-release), 0)
DEBUG_SYMBOL := -ggdb
endif

CLEAN_TARGET ?= obj

ifndef warn
WARNINGS := -w
else
WARNINGS := -W$(warn)
endif

ifdef rcscript
PROCESS_RC_FILE		:= @windres build/$(rcscript) -o obj/rc/cfg.o
CREATE_RC_FOLDER	:= @mkdir -p obj\rc
INCLUDE_RC_FILE		:= obj/rc/cfg.o
endif

ifdef macro
macro = -D$(macro)
endif

GUI_MODE ?= -mwindows

.PHONY: clean debug release all demo both test help

export HELP_MESSAGE

help:
	@echo "$$HELP_MESSAGE"

debug: build\$(src)
	@echo ""
	@echo "[--- START ---]"
	
	$(GET_TIME)
	
	@mkdir -p obj\$@
	
	@echo "[0/2] processing resources [$@]..."
	$(CREATE_RC_FOLDER)
	$(PROCESS_RC_FILE)
	
	@echo "[1/2] compiling [$@]..."
	@$(CXX) $(COMPILER_CONFIG) -Wall -Wpedantic $(SAFE_MATH) -pg -Og -ggdb3 $(SANITIZER_OPTIONS) -fno-omit-frame-pointer -D_DEBUG_OUTPUT_ -D$(macro) $(INCLUDES) -c build\$(src) -o obj/$@/$(name).o
	
	@echo "[2/2] linking libraries..."
	@$(CXX) -o res/$(name)_$@.exe obj/$@/$(name).o $(INCLUDE_RC_FILE) $(LINKER_CONFIG) -pg -Og $(LIBRARIES) $(SANITIZER_OPTIONS)
	
	@echo "Done!"
	$(MAKE_CLEAN)
	
	$(GET_TIME)
	
	@echo "[--- END ---]"
	@echo 


demo: build\$(src)
	@echo ""
	@echo "[--- START ---]"

	$(GET_TIME)
	
	@mkdir -p obj\$@
	
	@echo "[0/2] processing resources [$@]..."
	$(CREATE_RC_FOLDER)
	$(PROCESS_RC_FILE)
	
	@echo "[1/2] compiling [$@]..."
	@$(CXX) $(COMPILER_CONFIG) $(WARNINGS) $(OPTIMIZATIONS) -O$(optimize-lvl) $(DEBUG_SYMBOL) $(SAFE_MATH) $(OPENMP_ENABLED) $(INCLUDES) -D_DEMO_BUILD_ -D$(macro) -c build\$(src) -o obj/$@/$(name).o
	
	@echo "[2/2] linking libraries..."
	@$(CXX) -o res/$(name)_$@.exe obj/$@/$(name).o $(INCLUDE_RC_FILE) $(LINKER_CONFIG) -O$(optimize-lvl)  $(LIBRARIES) $(GUI_MODE)
	
	@echo "Done!"
	$(MAKE_CLEAN)
	
	$(GET_TIME)
	
	@echo "[--- END ---]"
	@echo 


release: build\$(src)
	@echo ""
	@echo "[--- START ---]"

	$(GET_TIME)
	
	@mkdir -p obj\$@
	
	@echo "[0/2] processing resources [$@]..."
	$(CREATE_RC_FOLDER)
	$(PROCESS_RC_FILE)
	
	@echo "[1/2] compiling [$@]..."
	@$(CXX) $(COMPILER_CONFIG) $(WARNINGS) $(OPTIMIZATIONS) -O$(optimize-lvl) $(DEBUG_SYMBOL) $(SAFE_MATH) $(OPENMP_ENABLED) $(INCLUDES) -D$(macro) -c build\$(src) -o obj/$@/$(name).o
	
	@echo "[2/2] linking libraries..."
	@$(CXX) -o res/$(name).exe obj/$@/$(name).o $(INCLUDE_RC_FILE) $(LINKER_CONFIG) -O$(optimize-lvl)  $(LIBRARIES) $(GUI_MODE)
	
	@echo "Done!"
	$(MAKE_CLEAN)
	
	$(GET_TIME)
	
	@echo "[--- END ---]"
	@echo 
	

test: debug release

preview: debug demo

both: demo release
	
all: debug demo release

pack-demo:
	@echo "Packing files..."
	@cd res
	@zip $(name).zip -r res/data/ res/shaders/ res/*.dll res/$(name)_demo.exe $(extra-files)
	@cd ..
	@echo "Done!"

pack-release:
	@echo "Packing files..."
	@cd res
	@zip $(name).zip -r res/data/ res/shaders/ res/*.dll res/$(name).exe $(extra-files)
	@cd ..
	@echo "Done!"
	
clean:
	@echo "Cleaning up..."
	@rm -rf $(CLEAN_TARGET)
	@echo "Done!"
