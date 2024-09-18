
ROOT	:= ../../..
INCLUDES := -I$(ROOT)/lib/stb

include ../../../commons.make

override prefix := $(concat, $(prefix), impl)

DEBUGCOMPCFG_C	:= $(C_COMPCFG) -Wall -Og -ggdb3 -fno-omit-frame-pointer $(DEBUGMODE)
C_COMPCFG	:= -fexpensive-optimizations -m64 -fms-extensions
COMPCFG_C	:= $(C_COMPCFG) -O$(o) $(OPTIMIZATIONS)

COMPILER	:= @$(CXX) $(INCLUDES)

lower = $(shell echo $(1) | tr A-Z a-z)
upper = $(shell echo $(1) | tr a-z A-Z)

ifdef gl-loader
ifeq ($(gl-loader),glew)
MAKE_GLOADER_D	:= @:
MAKE_GLOADER	:= @:
else
MAKE_GLOADER_D	= $(CC) -I$(ROOT)/lib/OpenGL/$(call upper,$(gl-loader))/include $(DEBUGCOMPCFG_C) -c $(gl-loader).c -o $(prefix).glapi.$@.o
MAKE_GLOADER	= $(CC) -I$(ROOT)/lib/OpenGL/$(call upper,$(gl-loader))/include $(COMPCFG_C) -c $(gl-loader).c -o $(prefix).glapi.$@.o
endif
else
gl-loader := glad
MAKE_GLOADER_D	= $(CC) -I$(ROOT)/lib/OpenGL/$(call upper,$(gl-loader))/include $(DEBUGCOMPCFG_C) -c $(gl-loader).c -o $(prefix).glapi.$@.o
MAKE_GLOADER	= $(CC) -I$(ROOT)/lib/OpenGL/$(call upper,$(gl-loader))/include $(COMPCFG_C) -c $(gl-loader).c -o $(prefix).glapi.$@.o
endif

.PHONY: debug release all
.ONESHELL:
.SHELLFLAGS = -ec

all: debug release

debug:
	$(call compile, stb)
	$(MAKE_GLOADER_D)


release:
	$(call compile, stb)
	$(MAKE_GLOADER)
