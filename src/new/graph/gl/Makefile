LEAN := -static -s

ROOT	:= ../../../..

include $(ROOT)/commons.make

override prefix := $(call concat, $(prefix), graph.gl)

INCLUDES := $(INC_SDL) $(INC_STB) $(INC_OPENGL) $(INC_GL_LOADER)

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
