ROOT	:= ../../..

include $(ROOT)/commons.make

override prefix := $(call concat, $(prefix), data)

INCLUDES := $(INC_CPPCODEC)

.PHONY: debug release all
.ONESHELL:
.SHELLFLAGS = -ec

all: debug release

debug:
	$(call compile, encdec)
	$(call compile, hash)

release:
	$(call compile, encdec)
	$(call compile, hash)
