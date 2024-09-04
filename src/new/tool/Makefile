define GET_TIME
@printf "\nTime: "
@date +\"%H:%M:%S\"
@echo ""
endef

ifdef openmp
export openmp
endif

prefix += .tool
export prefix

.PHONY: debug release all
.ONESHELL:
.SHELLFLAGS = -ec

all: debug release

debug:
	@cd archive
	@make $@
	@cd ..


release:
	@cd archive
	@make $@
	@cd ..
