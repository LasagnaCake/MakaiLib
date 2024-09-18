ROOT	:= ../../../../..

include $(ROOT)/commons.make

override prefix := $(call concat, $(prefix), mat)

INCLUDES := -I$(ROOT)/lib/SDL2-2.0.10/include -I$(ROOT)/lib/OpenGL -I$(ROOT)/$(GL_LOADER)

ifdef debug-release
RELEASEMODE := $(DEBUGMODE)
export debug-release
endif

export o?=3

COMPCFG	:= $(COMPILER_CONFIG) -O$(o) $(OPTIMIZATIONS) $(RELEASEMODE)
LINKCFG	:= $(LINKER_CONFIG) -O$(o) $(LIBRARIES) $(LEAN)

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
	$(COMPILER) $(DEBUGCOMPCFG) -c effect.cpp -o $(prefix).effect.gl.$@.o
	$(COMPILER) $(DEBUGCOMPCFG) -c materials.cpp -o $(prefix).materials.gl.$@.o


release:
	$(COMPILER) $(COMPCFG) -c effect.cpp -o $(prefix).effect.$@.o
	$(COMPILER) $(COMPCFG) -c materials.cpp -o $(prefix).materials.$@.o
