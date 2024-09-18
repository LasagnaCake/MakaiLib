include ../../../commons.make

define GET_TIME
@printf "\nTime: "
@date +\"%H:%M:%S\"
@echo ""
endef

override prefix := $(call concat $(prefix), tool)

.PHONY: debug release all
.ONESHELL:
.SHELLFLAGS = -ec

all: debug release

debug:
	@cd archive
	@make $@ prefix="$(prefix)"
	@cd ..


release:
	@cd archive
	@make $@ prefix="$(prefix)"
	@cd ..
