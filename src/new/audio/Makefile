ROOT	:= ../../..

include $(ROOT)/commons.make

INCLUDES := $(INC_SDL)

.PHONY: debug release all
.ONESHELL:
.SHELLFLAGS = -ec

all: debug release

debug:
	$(call compile, core)
	$(call compile, playable)
	$(call compile, music)
	$(call compile, sound)

release:
	$(call compile, core)
	$(call compile, playable)
	$(call compile, music)
	$(call compile, sound)
