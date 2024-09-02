define GET_TIME
@printf "\nTime: "
@date +\"%H:%M:%S\"
@echo ""
endef

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