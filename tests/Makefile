CC	:= gcc
CXX	:= g++

COMPILER_CONFIG	:= -m64 -std=gnu++20 -fms-extensions -I../output/include
DEBUGMODE := -DMAKAILIB_DEBUG

LIBREQS		:= -static-libstdc++ -static-libgcc
GRAPHREQS	:= -lopengl32
WINREQS		:= -lole32 -loleaut32 -limm32 -lwinmm -lversion -lpowrprof -lcomdlg32 -lsetupapi -lgdi32

testcmd = @$(CXX) -o output/$(1)/test.$(2).$(1).exe test.$(2).cpp $(COMPILER_CONFIG) ../output/lib/$(3)

dotest = \
	$(if $(findstring debug,$(1)),\
		$(call testcmd,$(strip $(1)),$(strip $(2)),libmakai.$(strip $(1)).a) $(DEBUGMODE),\
		$(call testcmd,$(strip $(1)),$(strip $(2)),libmakai.a) -mwindows\
	)

case = $(if $(findstring $(strip $(2)),$(strip $(1))),$(3))

params = \
	$(call case, $(1), 01,)\
	$(call case, $(1), 02,$(LIBREQS) $(GRAPHREQS))\

define mktest
	@echo "$(strip $(2))"
	$(call dotest, $(1), $(2)) $(call params, $(2))
endef

ifndef t
define tests
	$(call mktest, $(1), 01.compilation)
	$(call mktest, $(1), 02.app)
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
