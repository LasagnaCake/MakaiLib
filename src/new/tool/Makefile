define GET_TIME
@printf "\nTime: "
@date +\"%H:%M:%S\"
@echo ""
endef

ifdef openmp
export openmp
endif

prefix += .tool

.PHONY: debug release all
.ONESHELL:
.SHELLFLAGS = -ec

all: debug release

debug:
	@cd archive
	@make $@ prefix=$(prefix)
	@cd ..


release:
	@cd archive
	@make $@ prefix=$(prefix)
	@cd ..
