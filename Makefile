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

WINGARBAGE := -lole32 -loleaut32 -limm32 -lwinmm -lversion -lpowrprof -lcomdlg32 -lsetupapi -lgdi32

LIBRARIES := lib/SDL2-2.0.10/lib/libSDL2.dll.a
LIBRARIES += lib/SDL2-2.0.10/lib/libSDL2main.a
LIBRARIES += lib/SDL2-2.0.10/lib/libSDL2_mixer.dll.a
LIBRARIES += lib/cryptopp/lib/libcryptopp.a
LIBRARIES += lib/OpenGL/GLEW/lib/libglew32.dll.a
LIBRARIES += -lopengl32 -lgomp -lstdc++ -lgcc 
LIBRARIES += $(WINGARBAGE)

LINKER := @$(CXX)
LINKER := @ld

CONFIG := -static #--static-libstdc++ --static-libgcc

.PHONY: build-debug build-release link-debug link-release build-all link-all debug release copy-headers copy-o-debug copy-o-release all help
.ONESHELL:
.SHELLFLAGS = -ec

export HELP_MESSAGE

help:
	@echo "$$HELP_MESSAGE"

all: build-all up-all link-all

build-all: build-debug build-release

up-all: up-debug up-release

link-all: link-debug link-release

debug: build-debug up-debug link-debug

release: build-release up-release link-release

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
	@echo "Adding exterals..."
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
	@echo "Adding exterals..."
	@ar -M <makelib.release.mri
	@echo "Finalizing..."
	@ranlib output/lib/libmakai.a
	@rm -rf output/lib/st*
	@echo "Done!"
	@echo 
