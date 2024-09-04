CC	:= gcc
CXX	:= g++

define HELP_MESSAGE
endef

define GET_TIME
@printf "\nTime: "
@date +\"%H:%M:%S\"
@echo ""
endef

LEAN := -static -s

#WINGARBAGE := -lole32 -loleaut32 -limm32 -lwinmm -lversion -lpowrprof -lcomdlg32 -lsetupapi -lgdi32

SDL			:= lib/SDL2-2.0.10/lib/libSDL2.dll.a
SDLNET		:= lib/SDL2-2.0.10/lib/libSDL2_net.a
SDLMIXER	:= lib/SDL2-2.0.10/lib/libSDL2_mixer.dll.a
CRYPTOPP	:= lib/cryptopp/lib/libcryptopp.a
GLEW		:= lib/OpenGL/GLEW/lib/libglew32.dll.a

ifeq ($(wildcard obj/extern/extern.3p.a),)
CREATE_E3P := link-extern
endif

D := $

CONFIG := -static #--static-libstdc++ --static-libgcc

ifdef openmp
export openmp
endif

.PHONY: build-debug build-release up-debug up-release link-debug link-release build-all up-all link-all debug release copy-headers copy-o-debug copy-o-release all help link-extern
.ONESHELL:
.SHELLFLAGS = -ec

export HELP_MESSAGE

help:
	@echo "$$HELP_MESSAGE"

all: build-all up-all link-all

build-all: build-debug build-release

up-all: copy-headers copy-o-debug copy-o-release

link-all: $(CREATE_E3P) link-debug link-release

debug: build-debug up-debug $(CREATE_E3P) link-debug

release: build-release up-release $(CREATE_E3P) link-release

build-debug:
	@cd src/new
	@make debug
	@cd ../..

build-release:
	@cd src/new
	@make release
	@cd ../..

copy-headers:
	@echo "Copying headers..."
	@mkdir -p output/include
	@cd src/new
	@cp -r --parents *.hpp ../../output/include/
	@cp -r --parents **/*.hpp ../../output/include/
	@cp -r --parents **/**/*.hpp ../../output/include/
	@cp -r --parents **/**/**/*.hpp ../../output/include/
	#@cp -r --parents **/**/**/**/*.hpp ../../output/include/
	@cp -r --parents ctl/* ../../output/include/
	@cd ../..

copy-o-debug:
	@echo "Copying objects..."
	@mkdir -p obj/debug
	#@cp -r src/new/*.debug.o obj/debug/
	@cp -r src/new/**/*.debug.o obj/debug/
	@cp -r src/new/**/**/*.debug.o obj/debug/
	@cp -r src/new/**/**/**/*.debug.o obj/debug/
	#@cp -r src/new/**/**/**/**/*.debug.o obj/debug/

copy-o-release:
	@echo "Copying objects..."
	@mkdir -p obj/release
	#@cp -r src/new/*.release.o obj/release/
	@cp -r src/new/**/*.release.o obj/release/
	@cp -r src/new/**/**/*.release.o obj/release/
	@cp -r src/new/**/**/**/*.release.o obj/release/
	#@cp -r src/new/**/**/**/**/*.release.o obj/release/

up-debug: copy-headers copy-o-debug
	@echo "File copy done!"

up-release: copy-headers copy-o-release
	@echo "File copy done!"

link-debug:
	@echo "Creating lib folder..."
	@rm -rf output/lib/libmakai.debug.a
	@mkdir -p output/lib
	@echo "Building library..."
	@ar rcvs output/lib/libmakai.debug.a obj/debug/*.debug.o
	@echo "Adding externals..."
	@ar -M <makelib.debug.mri
	@echo "Finalizing..."
	@ranlib output/lib/libmakai.debug.a
	@rm -rf output/lib/st*
	@echo "Done!"
	@echo 

link-release:
	@echo "Creating lib folder..."
	@rm -rf output/lib/libmakai.a
	@mkdir -p output/lib
	@echo "Building library..."
	@ar rcvs output/lib/libmakai.a obj/release/*.release.o
	@echo "Adding externals..."
	@ar -M <makelib.release.mri
	@echo "Finalizing..."
	@ranlib output/lib/libmakai.a
	@rm -rf output/lib/st*
	@echo "Done!"
	@echo 

THIRD_PARTY_PREFIX := lib.3p

define addname
	@echo "Renaming [$(strip $(1))]..."
	@for file in *.o; do mv $$file $(THIRD_PARTY_PREFIX).$(strip $(1)).$$file; done
endef

define repack
	@echo "Repacking [$(strip $(1))]..."
	@cd $(strip $(1))
	@ar rcvs ../$(THIRD_PARTY_PREFIX).$(strip $(1)).a *.o
	@cd ..
endef

rp-glew-part = @ar rcvs ../$(THIRD_PARTY_PREFIX).glew.a $(THIRD_PARTY_PREFIX).glew.d00$(strip $(1))*.o

define rp-glew-pack10
	$(call rp-glew-part, $(strip $(1))0)
	$(call rp-glew-part, $(strip $(1))1)
	$(call rp-glew-part, $(strip $(1))2)
	$(call rp-glew-part, $(strip $(1))3)
	$(call rp-glew-part, $(strip $(1))4)
	$(call rp-glew-part, $(strip $(1))5)
	$(call rp-glew-part, $(strip $(1))6)
	$(call rp-glew-part, $(strip $(1))7)
	$(call rp-glew-part, $(strip $(1))8)
	$(call rp-glew-part, $(strip $(1))9)
endef

define repack-glew
	@echo "Repacking [glew]..."
	@cd glew
	$(call rp-glew-pack10, 0)
	$(call rp-glew-pack10, 1)
	$(call rp-glew-pack10, 2)
	$(call rp-glew-part, 30)
	$(call rp-glew-part, 31)
	$(call rp-glew-part, 32)
	$(call rp-glew-part, 33)
	$(call rp-glew-part, 34)
	$(call rp-glew-part, 35)
	$(call rp-glew-part, 36)
	$(call rp-glew-part, 37)
	$(call rp-glew-part, 38)
	@cd ..
endef

extract-extern:
	@echo "Creating lib folder..."
	@rm -rf obj/extern
	@mkdir -p obj/extern/sdl
	@mkdir -p obj/extern/sdl-net
	@mkdir -p obj/extern/sdl-mixer
	@mkdir -p obj/extern/glew
	@mkdir -p obj/extern/cryptopp
	@echo "Extracting objects..."
	@ar x $(SDL) --output "obj/extern/sdl"
	@ar x $(SDLNET) --output "obj/extern/sdl-net"
	@ar x $(SDLMIXER) --output "obj/extern/sdl-mixer"
	@ar x $(GLEW) --output "obj/extern/glew"
	@ar x $(CRYPTOPP) --output "obj/extern/cryptopp"

rename-sdl-mixer:
	@cd obj/extern/sdl-mixer
	@echo "Renaming [sdl-mixer]..."
	@for file in SDL2_mixer_dll_*.o; do mv -- "$$file" "$(THIRD_PARTY_PREFIX).sdl-mixer.$${file##*_}"; done

rename-extern:
	@echo "Renaming objects..."
	@cd obj/extern
	@cd sdl
	$(call addname, sdl)
	@cd ../sdl-net
	$(call addname, sdl-net)
	@cd ../sdl-mixer
	@echo "Renaming [sdl-mixer]..."
	@for file in SDL2_mixer_dll_*.o; do mv -- "$$file" "$(THIRD_PARTY_PREFIX).sdl-mixer.$${file##*_}"; done
	@cd ../glew
	$(call addname, glew)
	@cd ../cryptopp
	$(call addname, cryptopp)
	@cd ../../..

repack-extern:
	@cd obj/extern
	@echo "Re-combining libraries..."
	$(call repack, sdl)
	$(call repack, sdl-net)
	$(call repack, sdl-mixer)
	$(repack-glew)
	$(call repack, cryptopp)
	@cd ../..

combine-extern:
	@echo "Combining files..."
	@ar -M <makelib.extern.mri
	@ranlib obj/extern/extern.3p.a
	@rm -rf obj/extern/st*

link-extern: extract-extern rename-extern repack-extern combine-extern
	@echo "Done!"
