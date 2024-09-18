ROOT	:= ../../../../..

include $(ROOT)/commons.make

override prefix := $(call concat, $(prefix), mat)

INCLUDES := $(INC_SDL) $(INC_OPENGL) $(INC_GL_LOADER)

all: debug release

debug:
	$(call compile, effect)
	$(call compile, materials)

release:
	$(call compile, effect)
	$(call compile, materials)
