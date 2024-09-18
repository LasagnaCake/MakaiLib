import globals.make

CC	?= gcc
CXX	?= g++

lower = $(shell echo $(1) | tr A-Z a-z)
upper = $(shell echo $(1) | tr a-z A-Z)

concat = $(strip $(1)).$(strip $(2))

LEAN := -static -s

COMPILER_CONFIG	:= -fexpensive-optimizations -m64 -std=gnu++20 -fconcepts -fcoroutines -fms-extensions

ifdef openmp
USE_OPENMP := -fopenmp -openmp -ftree-parallelize-loops=128
export openmp
endif

ifdef no-buffers
NO_BUFFERS := -DMAKAILIB_DO_NOT_USE_BUFFERS
export no-buffers
endif

OPTIMIZATIONS	:= $(USE_OPENMP) -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize

DEBUGMODE		:= -DMAKAILIB_DEBUG -DCTL_DEBUG -DNDEBUG

ifdef debug-release
RELEASEMODE := $(DEBUGMODE)
export debug-release
else
RELEASEMODE := $(LEAN)
endif

ifdef gl-loader
export gl-loader
else
export gl-loader:=glad
endif

GL_LOADER_FLAG := -DMAKAILIB_GL_LOADER=MAKAILIB_USE_$(call upper,$(gl-loader))

INC_GL_LOADER	= -I$(ROOT)/lib/OpenGL/$(call upper,$(gl-loader))/include $(GL_LOADER_FLAG)
INC_SDL			= -I$(ROOT)/lib/SDL2-2.0.10/include
INC_OPENGL		= -I$(ROOT)/lib/OpenGL
INC_STB			= -I$(ROOT)/lib/stb
INC_CPPCODEC	= -I$(ROOT)/lib/cppcodec-0.2
INC_CRYPTOPP	= -I$(ROOT)/lib/cryptopp/include

DEBUG_CONFIG	:= $(COMPILER_CONFIG) -O$(o) $(OPTIMIZATIONS) $(RELEASEMODE)
RELEASE_CONFIG	:= $(COMPILER_CONFIG) -Wall -Wpedantic -Og -ggdb3 -fno-omit-frame-pointer $(DEBUGMODE)

COMPILER = @$(CXX) $(INCLUDES)

compile-release	= $(COMPILER) $(DEBUG_CONFIG) -c $(strip $(1)).cpp -o $(prefix).$(strip $(1)).$@.o
compile-debug	= $(COMPILER) $(RELEASE_CONFIG) -c $(strip $(1)).cpp -o $(prefix).$(strip $(1)).$@.o

compile = \
	$(if $(findstring debug,$@),\
		$(call compile-debug, $(1)),\
		$(call compile-release, $(1))\
	)

define GET_TIME
@printf "\nTime: "
@date +\"%H:%M:%S\"
@echo ""
endef

define submake-impl
@cd $(1)
@make $@ prefix="$(strip $(2))"
@cd ..
endef

submake = $(call submake-impl, $(1), $(prefix))

export compile
export submake
export upper
export lower
export concat
export DEBUGMODE
export CC
export CXX
export GL_LOADER
export GET_TIME
export INC_GL_LOADER
export INC_OPENGL
export INC_SDL
export INC_STB
export INC_CPPCODEC
export INC_CRYPTOPP
export NO_BUFFERS
