CC	:= gcc
CXX	:= g++

LEAN := -static -s

ROOT := ../..

COMPILER_CONFIG	:= -m64 -std=gnu++20 -fms-extensions -I$(ROOT)/output/include
DEBUGMODE := -DMAKAILIB_DEBUG -fconcepts-diagnostics-depth=4 -Wpedantic

LIBREQS		:= -static-libstdc++ -static-libgcc
GRAPHREQS	:= -lopengl32
WINREQS		:= -lole32 -loleaut32 -limm32 -lwinmm -lversion -lpowrprof -lcomdlg32 -lsetupapi -lgdi32

o?=3

testcmd = @$(CXX) -O$(o) -o output/$(1)/test.$(2).$(1).exe test.$(2).*.cpp $(COMPILER_CONFIG) $(ROOT)/output/lib/$(3)

NOCMD := -mwindows

ifdef debug-release
RELEASEMODE := $(DEBUGMODE)
else
RELEASEMODE := $(NOCMD) $(LEAN)
endif

dotest = \
	$(if $(findstring debug,$(1)),\
		$(call testcmd,$(strip $(1)),$(strip $(2)),libmakai.$(strip $(1)).a) $(DEBUGMODE),\
		$(call testcmd,$(strip $(1)),$(strip $(2)),libmakai.a) $(RELEASEMODE)\
	)

case = $(if $(findstring $(strip $(2)),$(strip $(1))),$(3))

params = \
	$(call case, $(1), 01,$(LIBREQS))\
	$(call case, $(1), 02,$(LIBREQS) $(GRAPHREQS))\
	$(call case, $(1), 03,$(LIBREQS) $(GRAPHREQS))\
	$(call case, $(1), 04,$(LIBREQS) $(GRAPHREQS))\

define mktest
	@echo "$(strip $(2))"
	$(call dotest, $(1), $(2)) $(call params, $(2))
endef

ifndef t
define tests
	$(call mktest, $(1), 01)
	$(call mktest, $(1), 02)
	$(call mktest, $(1), 03)
	$(call mktest, $(1), 04)
endef
else
define tests
	$(call mktest, $(1), $(t))
endef
endif

all: debug release

debug:
	@echo "Building tests [$@]..."
	@mkdir -p output/$@
	$(call tests, $@)
	@echo "Done!"
	@echo 

release:
	@echo "Building tests [$@]..."
	@mkdir -p output/$@
	$(call tests, $@)
	@echo "Done!"
	@echo 
