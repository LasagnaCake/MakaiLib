LEAN := -static -s

ROOT	:= ../../../..

include $(ROOT)/commons.make

override prefix := $(call concat, $(prefix), graph.gl)

INCLUDES := -I$(ROOT)/lib/SDL2-2.0.10/include -I$(ROOT)/lib/OpenGL -I$(ROOT)/$(GL_LOADER) -I$(ROOT)/lib/stb

.PHONY: debug release all
.ONESHELL:
.SHELLFLAGS = -ec

all: debug release

debug:
	$(call compile, core)
	$(call compile, global)
	$(call compile, vertex)
	$(call compile, uniform)
	$(call compile, blend)
	$(call compile, color)
	$(call compile, image)
	$(call compile, texture)
	$(call compile, shader)
	
	@cd renderer
	@make $@ prefix="$(prefix)"
	@cd ..

	@cd material
	@make $@ prefix="$(prefix)"
	@cd ..


release:
	$(call compile, core)
	$(call compile, global)
	$(call compile, vertex)
	$(call compile, uniform)
	$(call compile, blend)
	$(call compile, color)
	$(call compile, image)
	$(call compile, texture)
	$(call compile, shader)
	
	@cd renderer
	@make $@ prefix="$(prefix)"
	@cd ..

	@cd material
	@make $@ prefix="$(prefix)"
	@cd ..
