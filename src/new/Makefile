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

	@cd impl
	@make $@
	@cd ..

	@cd tool
	@make $@
	@cd ..

	@cd file
	@make $@
	@cd ..

	@cd data
	@make $@
	@cd ..

	@cd audio
	@make $@
	@cd ..

	@cd graph/gl
	@make $@
	@cd ../..

	@cd core
	@make $@
	@cd ..

	@echo "</subsystems>"
	@echo ""

	@echo "Done!"
	
	$(GET_TIME)
	@echo "[--- END ---]"
	@echo ""
	@echo "[------------------------------------]"
	@echo "[------------------------------------]"
	@echo "[---!!!                        !!!---]"
	@echo "[--!!!  Full compilation done!  !!!--]"
	@echo "[---!!!                        !!!---]"
	@echo "[------------------------------------]"
	@echo "[------------------------------------]"
	@echo ""
	@echo 
	@echo 
	@echo 
	@echo 
	@echo 


release:
	@echo ""
	@echo "[--- START ---]"
	$(GET_TIME)
	
	@echo "[1/2] compiling [$@]..."

	@cd impl
	@make $@
	@cd ..

	@cd tool
	@make $@
	@cd ..

	@cd file
	@make $@
	@cd ..

	@cd data
	@make $@
	@cd ..

	@cd audio
	@make $@
	@cd ..

	@cd graph/gl
	@make $@
	@cd ../..

	@cd core
	@make $@
	@cd ..

	@echo "Done!"
	
	$(GET_TIME)
	@echo "[--- END ---]"
	@echo ""
	@echo "[------------------------------------]"
	@echo "[------------------------------------]"
	@echo "[---!!!                        !!!---]"
	@echo "[--!!!  Full compilation done!  !!!--]"
	@echo "[---!!!                        !!!---]"
	@echo "[------------------------------------]"
	@echo "[------------------------------------]"
	@echo ""
	@echo 
	@echo 
	@echo 
	@echo 