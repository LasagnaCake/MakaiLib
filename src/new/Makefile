define GET_TIME
@printf "\nTime: "
@date +\"%H:%M:%S\"
@echo ""
endef

ifdef openmp
export openmp
endif

.PHONY: debug release all
.ONESHELL:
.SHELLFLAGS = -ec

all: debug release

debug:
	@echo ""
	@echo "[-------------------------------------]"
	@echo "[-------------------------------------]"
	@echo "[---!!!                         !!!---]"
	@echo "[--!!!  Full compilation start!  !!!--]"
	@echo "[---!!!                         !!!---]"
	@echo "[-------------------------------------]"
	@echo "[-------------------------------------]"
	@echo ""
	$(GET_TIME)
	@echo ""
	@echo "TYPE: $@"
	@echo ""
	@echo "[--- START ---]"
	
	@echo "[1/2] compiling [$@]..."

	@echo ""
	@echo "<systems>"
	@echo ""

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

	@echo ""
	@echo "</systems>"
	@echo ""

	@echo "Done!"
	
	@echo "[--- END ---]"
	@echo ""
	@echo "TYPE: $@"
	@echo ""
	$(GET_TIME)
	@echo ""
	@echo "[------------------------------------]"
	@echo "[------------------------------------]"
	@echo "[---!!!                        !!!---]"
	@echo "[--!!!  Full compilation done!  !!!--]"
	@echo "[---!!!                        !!!---]"
	@echo "[------------------------------------]"
	@echo "[------------------------------------]"
	@echo ""


release:
	@echo ""
	@echo "[-------------------------------------]"
	@echo "[-------------------------------------]"
	@echo "[---!!!                         !!!---]"
	@echo "[--!!!  Full compilation start!  !!!--]"
	@echo "[---!!!                         !!!---]"
	@echo "[-------------------------------------]"
	@echo "[-------------------------------------]"
	@echo ""
	$(GET_TIME)
	@echo ""
	@echo "TYPE: $@"
	@echo ""
	@echo "[--- START ---]"
	
	@echo "[1/2] compiling [$@]..."

	@echo ""
	@echo "<systems>"
	@echo ""

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

	@echo ""
	@echo "</systems>"
	@echo ""

	@echo "Done!"
	
	
	@echo "[--- END ---]"
	@echo ""
	@echo "TYPE: $@"
	@echo ""
	$(GET_TIME)
	@echo ""
	@echo "[------------------------------------]"
	@echo "[------------------------------------]"
	@echo "[---!!!                        !!!---]"
	@echo "[--!!!  Full compilation done!  !!!--]"
	@echo "[---!!!                        !!!---]"
	@echo "[------------------------------------]"
	@echo "[------------------------------------]"
	@echo ""