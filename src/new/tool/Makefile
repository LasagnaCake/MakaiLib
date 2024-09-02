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
	@echo ""
	@echo "[--- START ---]"
	$(GET_TIME)
	
	@echo "[1/2] compiling [$@]..."

	@echo ""
	@echo "<subsystems>"

	@cd archive
	@make $@
	@cd ..

	@echo "</subsystems>"
	@echo ""

	@echo "Done!"
	
	$(GET_TIME)
	@echo "[--- END ---]"
	@echo 


release:
	@echo ""
	@echo "[--- START ---]"
	$(GET_TIME)
	
	@echo "[1/2] compiling [$@]..."

	@cd archive
	@make $@
	@cd ..

	@echo "Done!"
	
	$(GET_TIME)
	@echo "[--- END ---]"
	@echo 