import ../../commons.make

define GET_TIME
@printf "\nTime: "
@date +\"%H:%M:%S\"
@echo ""
endef

override prefix := $(call concat $(prefix), makai)

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
	@make $@ prefix="$(prefix)"
	@cd ..

	@cd tool
	@make $@ prefix="$(prefix)"
	@cd ..

	@cd file
	@make $@ prefix="$(prefix)"
	@cd ..

	@cd data
	@make $@ prefix="$(prefix)"
	@cd ..

	@cd audio
	@make $@ prefix="$(prefix)"
	@cd ..

	@cd graph/gl
	@make $@ prefix="$(prefix)"
	@cd ../..

	@cd core
	@make $@ prefix="$(prefix)"
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
	@make $@ prefix="$(prefix)"
	@cd ..

	@cd tool
	@make $@ prefix="$(prefix)"
	@cd ..

	@cd file
	@make $@ prefix="$(prefix)"
	@cd ..

	@cd data
	@make $@ prefix="$(prefix)"
	@cd ..

	@cd audio
	@make $@ prefix="$(prefix)"
	@cd ..

	@cd graph/gl
	@make $@ prefix="$(prefix)"
	@cd ../..

	@cd core
	@make $@ prefix="$(prefix)"
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
