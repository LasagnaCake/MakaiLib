sinclude options.make

export space := 

export CC	?= gcc
export CXX	?= g++

export lower =$(shell echo $(1) | tr A-Z a-z)
export upper =$(shell echo $(1) | tr a-z A-Z)

export concat =$(strip $(1)).$(strip $(2))

export LEAN := -static -s

COMPILER_CONFIG	:= -fexpensive-optimizations -m64 -std=gnu++20 -fconcepts -fcoroutines -fms-extensions

ifdef openmp
export USE_OPENMP := -fopenmp -openmp -ftree-parallelize-loops=128
endif

ifdef no-buffers
export NO_BUFFERS := -DMAKAILIB_DO_NOT_USE_BUFFERS
endif

OPTIMIZATIONS	:= $(USE_OPENMP) -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize

export DEBUGMODE	:= -DMAKAILIB_DEBUG -DCTL_DEBUG -DNDEBUG

ifdef debug-release
export RELEASEMODE := $(DEBUGMODE)
else
export RELEASEMODE := $(LEAN)
endif

GL_LOADER_FLAG := -DMAKAILIB_GL_LOADER=MAKAILIB_USE_$(call upper,$(gl-loader))

export INC_GL_LOADER	= -I$(ROOT)/lib/OpenGL/$(call upper,$(gl-loader))/include $(GL_LOADER_FLAG)
export INC_SDL			= -I$(ROOT)/lib/SDL2-2.0.10/include
export INC_OPENGL		= -I$(ROOT)/lib/OpenGL
export INC_STB			= -I$(ROOT)/lib/stb
export INC_CPPCODEC		= -I$(ROOT)/lib/cppcodec-0.2
export INC_CRYPTOPP		= -I$(ROOT)/lib/cryptopp/include

DEBUG_CONFIG	:= $(COMPILER_CONFIG) -O$(o) $(OPTIMIZATIONS) $(RELEASEMODE)
RELEASE_CONFIG	:= $(COMPILER_CONFIG) -Wall -Wpedantic -Og -ggdb3 -fno-omit-frame-pointer $(DEBUGMODE)

COMPILER = @$(CXX) $(INCLUDES)

compile-release	= $(COMPILER) $(DEBUG_CONFIG) -c $(strip $(1)).cpp -o $(prefix).$(strip $(1)).$@.o
compile-debug	= $(COMPILER) $(RELEASE_CONFIG) -c $(strip $(1)).cpp -o $(prefix).$(strip $(1)).$@.o

export compile = \
	$(if $(findstring debug,$@),\
		$(call compile-debug, $(1)),\
		$(call compile-release, $(1))\
	)

define GET_TIME
@printf "\nTime: "
@date +\"%H:%M:%S\"
@echo ""
endef
export GET_TIME

export leave = $(subst $(space),,$(filter ../,$(subst /, ../ ,$(strip $(1)))))

define submake-impl
@cd $(1)
@make $@ prefix="$(strip $(2))"
@cd $(call leave, $(1))..
endef

export submake = $(call submake-impl, $(1), $(prefix))
