ROOT := ../../../../..
include $(ROOT)/make/core.make

override prefix := $(call concat, $(prefix), mat)

INCLUDES := $(INC_SDL) $(INC_OPENGL)

all: debug release

debug:
	$(call compile, effect)
	$(call compile, materials)

release:
	$(call compile, effect)
	$(call compile, materials)
