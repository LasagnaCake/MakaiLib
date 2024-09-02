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
LIBRARIES += $(WINGARBAGE) -lopengl32 -lgomp

LINKER := @$(CXX)

.PHONY: build-debug build-release link-debug link-release build-all link-all debug release all help
.ONESHELL:
.SHELLFLAGS = -ec

export HELP_MESSAGE

help:
	@echo "$$HELP_MESSAGE"

all: build-all link-all

build-all: build-debug build-release

link-all: link-debug link-release

debug: build-debug link-debug

release: build-release link-release

build-debug:
	@cd src/new
	@make debug
	@cd ../..

build-release:
	@cd src/new
	@make debug
	@cd ../..

copy-includes:
	@mkdir -p output/include
	@cp -r -v "src/new/*.hpp" "output/include"

copy-o-debug:
	@mkdir -p obj
	@cp --parents `find -name \*.debug.o` obj

copy-o-debug:
	@mkdir -p obj
	@cp --parents `find -name \*.release.o` obj

link-debug: copy-includes copy-o-debug
	@mkdir -p output/lib
	@ld -o output/lib/libmakai.debug.a obj/*.debug.o --whole-archive $(LIBRARIES)
	@ranlib output/lib/libmakai.debug.a
	@echo 

link-release: copy-includes copy-o-release
	@mkdir -p output/lib
	@ld -o output/lib/libmakai.debug.a obj/*.debug.o $(LEAN) --whole-archive $(LIBRARIES)
	@ranlib output/lib/libmakai.a
	@echo 