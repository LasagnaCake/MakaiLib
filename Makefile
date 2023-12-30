define HELP_MESSAGE

Supported targets:
>   debug   : Builds enabling debug options, and with all warnings enabled (+ pedantic)
>   demo    : Builds enabling optimizations, and with the _DEMO_BUILD_ macro defined
>   release : Builds enabling optimizations
>   test    : Builds both debug and release
>   both    : Builds both release and demo
>   all     : Builds all options (debug, demo and release)
>   pack-*  : Exports the selected build target into a game package

Supported options:
>   name          = [ value ]        : Spec. the name of the output file       ( DEFAULT: program  )

Build-exclusive options:
>   src           = [ value ]        : Spec. the source file                   ( DEFAULT: main.cpp )
>   warn          = [ value | none ] : Spec. the warning to enable             ( DEFAULT: none     )
>   use-openmp    = [ 1 | 0 ]        : Spec. whether ro enable OpenMP          ( DEFAULT: 0        )
>   optimize-lvl  = [ 0 - 3 | g ]    : Spec. the optimization level            ( DEFAULT: 2        )
>   debug-profile = [ 1 | 0 ]        : Spec. whether to enable gmon            ( DEFAULT: 0        )
>   keep-o-files  = [ 1 | 0 ]        : Spec. if .o files should be kept        ( DEFAULT: 0        )
>   macro         = [ value | none ] : Spec. a macro to be defined             ( DEFAULT: none     )
>   meth          = [ 1 | 0 ]        : Spec. whether to enable fast math       ( DEFAULT: 0        )
>   sath          = [ 1 | 0 ]        : Spec. whether to enable safe math       ( DEFAULT: 0        )
>   debug-release = [ 1 | 0 ]        : Spec. whether to enable -g on release   ( DEFAULT: 0        )
>   rcscript      = [ value | none ] : Spec. the location of a .rc file        ( DEFAULT: none     )

Pack-exclusive options:
>   extra-files   = [ value | none ] : Spec. extra files to pack in zip file   ( DEFAULT: none     )
>   extra-progs   = [ value | none ] : Spec. extra programs to pack in zip     ( DEFAULT: none     )
>   data-folder   = [ value | none ] : Spec. data folder to use instead        ( DEFAULT: none     )
>   over-debug    = [ 1 | 0 ]        : Spec. whether to overw. debug data/     ( DEFAULT: 0        )
>   over-demo     = [ 1 | 0 ]        : Spec. whether to overw. demo data/      ( DEFAULT: 0        )
>   over-release  = [ 1 | 0 ]        : Spec. whether to overw. release data/   ( DEFAULT: 0        )
>   no-archive    = [ 1 | 0 ]        : Spec. whether to not compact data/      ( DEFAULT: 0        )
>   archive-pass  = [ value | none ] : Spec. the archive's password            ( DEFAULT: none     )
>   archive-ext   = [ value | none ] : Spec. the archive's file extension      ( DEFAULT: arc      )

NOTES:
(Safe, in this case, means 'IEEE compliant'.)
>   meth       : Enables FAST, but IMPRECISE and UNSAFE floating point operations.
>   sath       : Enables SAFE math. If this one is enabled, METH is disabled.
>   rcfile     : Must be located within the 'build/' folder.
>   name       : For targers "debug" and "demo", "_debug" and "_demo" gets appended at the end of
                  the file, respectively.

Pack:
>   name         : Both the name used for the output zip file, and the name of the .exe, minus the
                    compilation target name MUST be the same name set while building.
>   extra-progs  : Must be Separate executables.
>   extra-files  : Must be the relative path to a folder to copy its contents from.
>   data-folder  : Must be the relative path to a folder containing a "data/" folder to use its
                    contents from.
>   over-*       : Only enabled if previous is set. specifies if corresponding target's "data/"
                    folder should be replaced. Only used for multi-target pakings.
>   no-archive   : Specifies whether the data/ folder is not compacted into an archive. Useful if
                    resources are to be shared. Generally, loading from a folder is faster than
                    loading from an archive, but does not offer the encryption an archive provides.
>   archive-pass : Must not contain spaces. ASCII characters only.

endef

define GET_TIME
@printf "\nTime: "
@date +\"%H:%M:%S\"
@echo ""
endef

src				?= main.cpp
name			?= program
debug-profile	?= 0
keep-o-files	?= 0
use-openmp		?= 0
meth			?= 0
sath			?= 0
debug-release	?= 0
over-debug		?= 0
over-demo		?= 0
over-release	?= 0
no-archive		?= 0
archive-ext		?= arc

CC 	?= gcc
CXX ?= g++

COMPILER_CONFIG	:= -fexpensive-optimizations -flto -m64 -std=gnu++20 -fcoroutines -fms-extensions
LINKER_CONFIG	:= -flto -static-libstdc++ -static-libgcc -static -m64 -fms-extensions

ifeq ($(sath), 1)
meth				:= 0
SAFE_MATH			:= -frounding-math -fsignaling-nans
endif

ifeq ($(meth), 1)
MATH_OPTIMIZATIONS	:= -ffast-math -fsingle-precision-constant
endif

OPTIMIZATIONS	:= $(MATH_OPTIMIZATIONS)

ifeq ($(use-openmp), 1)
OPTIMIZATIONS	+= -fopenmp -openmp -ftree-parallelize-loops=128 -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-vectorize
OPENMP_ENABLED	:= -D_USE_OPENMP_
optimize-lvl	:= g
else
optimize-lvl	?= 2
endif

ZIPINC	:= -Ilib/zip_utils/include
#ZIPINC	:= -Ilib/libzippp/include
#ZIPINC	:= -Ilib/bit7z/include

ZIPLIB	:= lib/zip_utils/lib/libziputils.a
#ZIPLIB	:= lib/libzippp/lib/libzippp.a
#ZIPLIB	:= lib/bit7z/lib/x64_custom/libbit7z64.a

INCLUDES		:= -Ilib -Isrc -Ilib/SDL2-2.0.10/include -Ilib/OpenGL -Ilib/OpenGL/GLEW/include -Ilib/cute_headers -Ilib/stb -Ilib/jsoncpp-3.11.2/include -Ilib/cppcodec-0.2 $(ZIPINC)

WINGARBAGE		:= -lole32 -loleaut32 -limm32 -lwinmm -lversion -lpowrprof -lcomdlg32 -lsetupapi -lgdi32

LIBRARIES		:= $(ZIPLIB) lib/SDL2-2.0.10/lib/libSDL2.dll.a lib/SDL2-2.0.10/lib/libSDL2main.a lib/SDL2-2.0.10/lib/libSDL2_mixer.dll.a lib/OpenGL/GLEW/lib/libglew32.dll.a $(WINGARBAGE) -lopengl32 -lgomp

# This doesn't work. Oh well.
# SANITIZER_OPTIONS := -fsanitize=leak -fsanitize=memory

# TODO: Clean up this mess

ifneq ($(debug-profile), 0)
GMON_OUT := -lgmon
endif

ifeq ($(keep-o-files), 0)
MAKE_CLEAN := @make clean CLEAN_TARGET=obj/$@ --no-print-directory -j1
endif

ifneq ($(debug-release), 0)
DEBUG_SYMBOL := -ggdb
endif

CLEAN_TARGET ?= obj

ifndef warn
WARNINGS := -w
else
WARNINGS := -W$(warn)
endif

ifdef rcscript
WINRC				:= @windres
RC_FILE_DIRS		:= build/$(rcscript) -o obj/rc/cfg.o
CREATE_RC_FOLDER	:= @mkdir -p obj/rc
INCLUDE_RC_FILE		:= obj/rc/cfg.o
else
WINRC				:= @:
endif

ifdef macro
DEFINE_MACRO = -D$(macro)
endif

ifeq ($(no-archive), 1)
DISABLE_ARCHIVE = -D_ARCHIVE_SYSTEM_DISABLED_
endif

DATA_FOLDER := res/data/
DEBUG_DATA		:= $(DATA_FOLDER)
DEMO_DATA		:= $(DATA_FOLDER)
RElEASE_DATA	:= $(DATA_FOLDER)

PACK_TARGET := $(filter pack-%,$(MAKECMDGOALS))

ifeq ($(PACK_TARGET), pack-debug)
over-debug := 1
endif
ifeq ($(PACK_TARGET), pack-demo)
over-demo := 1
endif
ifeq ($(PACK_TARGET), pack-release)
over-release := 1
endif

ifdef data-folder
DATA_FOLDER := $(data-folder)/data/
endif

ifeq ($(over-debug), 1)
DEBUG_DATA := $(DATA_FOLDER)
endif
ifeq ($(over-demo), 1)
DEMO_DATA := $(DATA_FOLDER)
endif
ifeq ($(over-release), 1)
RELEASE_DATA := $(DATA_FOLDER)
endif

# Red bold
COLOR_ERR	:= \033[1;91m
# Yellow bold
COLOR_WARN	:= \033[1;93m
# Reset color
COLOR_NONE	:= \033[0m

define ZIPWARNMSG
	@echo ""
	@echo -e "$(COLOR_WARN)!!! WARNING !!!$(COLOR_NONE)"
	@echo "7-Zip not found. Using zip instead."
	@echo "Please install p7zip, as it provides stronger encryption."
	@echo "If you're using MSYS, simply install it via \"pacman -S p7zip\"."
	@echo -e "$(COLOR_WARN)!!! WARNING !!!$(COLOR_NONE)"
	@echo ""
endef

#-r
ifneq (, $(shell which 7za))
WARNABOUT7ZIP := @:
ZIPCMD := @7za a -tzip -mem=AES256
ARCPASS := -p"$(archive-pass)"
else
WARNABOUT7ZIP := $(ZIPWARNMSG)
ZIPCMD := @zip -r
ARCPASS := -P $(archive-pass)
endif

ifdef archive-pass
ARCZIPCMD := $(ZIPCMD) $(ARCPASS)
else
ARCZIPCMD := $(ZIPCMD)
endif

ifeq ($(no-archive), 1)
DATA_ARCHIVE	:= @:
DATA_REMOVE		:= @:
else
DATA_ARCHIVE	:= $(ARCZIPCMD) data.$(archive-ext) data
DATA_REMOVE		:= @rm -rf data
endif

GUI_MODE ?= -mwindows

.PHONY: clean debug release all demo both test help pack-debug pack-release pack-all pack-demo pack-both pack-test pack-help
.ONESHELL:
.SHELLFLAGS = -ec

export HELP_MESSAGE

help:
	@echo "$$HELP_MESSAGE"


debug: build/$(src)
	@echo ""
	@echo "[--- START ---]"
	
	$(GET_TIME)
	
	@mkdir -p obj/$@
	
	@echo "[0/2] processing resources [$@]..."
	$(CREATE_RC_FOLDER)
	$(WINRC) -D_DEBUG_OUTPUT_ $(RC_FILE_DIRS)
	
	@echo "[1/2] compiling [$@]..."
	@$(CXX) $(COMPILER_CONFIG) -Wall -Wpedantic $(SAFE_MATH) -pg -Og -ggdb3 $(SANITIZER_OPTIONS) -fno-omit-frame-pointer -D_DEBUG_OUTPUT_ $(DEFINE_MACRO) $(INCLUDES) -D_ARCHIVE_SYSTEM_DISABLED_ -c build/$(src) -o obj/$@/$(name).o
	
	@echo "[2/2] linking libraries..."
	@$(CXX) -o res/$(name)_$@.exe obj/$@/$(name).o $(INCLUDE_RC_FILE) $(LINKER_CONFIG) -pg -Og $(LIBRARIES) $(SANITIZER_OPTIONS)
	
	@echo "Done!"
	$(MAKE_CLEAN)
	
	$(GET_TIME)
	
	@echo "[--- END ---]"
	@echo 


demo: build/$(src)
	@echo ""
	@echo "[--- START ---]"

	$(GET_TIME)
	
	@mkdir -p obj/$@
	
	@echo "[0/2] processing resources [$@]..."
	$(CREATE_RC_FOLDER)
	$(WINRC) -D_DEMO_BUILD_ $(RC_FILE_DIRS)
	
	@echo "[1/2] compiling [$@]..."
	@$(CXX) $(COMPILER_CONFIG) $(WARNINGS) $(OPTIMIZATIONS) -O$(optimize-lvl) $(DEBUG_SYMBOL) $(SAFE_MATH) $(OPENMP_ENABLED) $(INCLUDES) -D_DEMO_BUILD_ $(DEFINE_MACRO) -c build/$(src) -o obj/$@/$(name).o
	
	@echo "[2/2] linking libraries..."
	@$(CXX) -o res/$(name)_$@.exe obj/$@/$(name).o $(INCLUDE_RC_FILE) $(LINKER_CONFIG) -O$(optimize-lvl)  $(LIBRARIES) $(GUI_MODE)
	
	@echo "Done!"
	$(MAKE_CLEAN)
	
	$(GET_TIME)
	
	@echo "[--- END ---]"
	@echo 


release: build/$(src)
	@echo ""
	@echo "[--- START ---]"

	$(GET_TIME)
	
	@mkdir -p obj/$@
	
	@echo "[0/2] processing resources [$@]..."
	$(CREATE_RC_FOLDER)
	$(WINRC) $(RC_FILE_DIRS)
	
	@echo "[1/2] compiling [$@]..."
	@$(CXX) $(COMPILER_CONFIG) $(WARNINGS) $(OPTIMIZATIONS) -O$(optimize-lvl) $(DEBUG_SYMBOL) $(SAFE_MATH) $(OPENMP_ENABLED) $(INCLUDES) $(DEFINE_MACRO) -c build/$(src) -o obj/$@/$(name).o
	
	@echo "[2/2] linking libraries..."
	@$(CXX) -o res/$(name).exe obj/$@/$(name).o $(INCLUDE_RC_FILE) $(LINKER_CONFIG) -O$(optimize-lvl)  $(LIBRARIES) $(GUI_MODE)
	
	@echo "Done!"
	$(MAKE_CLEAN)
	
	$(GET_TIME)
	
	@echo "[--- END ---]"
	@echo 
	

test: debug release


preview: debug demo


both: demo release
	
	
all: debug demo release


pack-debug:
	$(WARNABOUT7ZIP)

	@echo ""
	@echo "[--- START ---]"

	$(GET_TIME)
	
	@echo "Packing files for '$@'..."
	@echo "[0/3] Creating folders..."
	@mkdir -p packed
	@mkdir -p packed/$(name)_debug
	
	@echo "[1/3] Copying contents..."
	@cp -r -v $(DEBUG_DATA) packed/$(name)_debug
	@cp -r -v res/shaders/ packed/$(name)_debug
	@cp -r -v res/subsys/ packed/$(name)_debug
	@cp -r -v res/*.dll packed/$(name)_debug
	@cp -r -v res/$(name)_debug.exe packed/$(name)_debug
	@cp -r -v $(extra-progs) packed/$(name)_debug
	@cp -r -v $(extra-files)/* packed/$(name)_debug
	
	@echo "[2/3] Zipping files..."
	
	@cd packed
	
	$(ZIPCMD) $(name)_debug.zip $(name)_debug
	
	@cd ..
	
	@echo "[3/3] Done!"
	@rm -rf packed/$(name)_debug
	
	$(GET_TIME)
	
	@echo "[--- END ---]"
	@echo 
	
	$(WARNABOUT7ZIP)
	

pack-demo:
	$(WARNABOUT7ZIP)

	@echo ""
	@echo "[--- START ---]"

	$(GET_TIME)
	
	@echo "Packing files for '$@'..."
	@echo "[0/3] Creating folders..."
	@mkdir -p packed
	@mkdir -p packed/$(name)_demo
	
	@echo "[1/3] Copying contents..."
	@cp -r -v $(DEMO_DATA) packed/$(name)_demo
	@cp -r -v res/shaders/ packed/$(name)_demo
	@cp -r -v res/subsys/ packed/$(name)_demo
	@cp -r -v res/*.dll packed/$(name)_demo
	@cp -r -v res/$(name)_demo.exe packed/$(name)_demo
	@cp -r -v $(extra-progs) packed/$(name)_demo
	@cp -r -v $(extra-files)/* packed/$(name)_demo
	
	@echo "[2/3] Zipping files..."
	
	@cd packed
	
	@cd $(name)_demo
	$(DATA_ARCHIVE)
	$(DATA_REMOVE)
	@cd ..
	
	$(ZIPCMD) $(name)_demo.zip $(name)_demo
	
	@cd ..
	
	@echo "[3/3] Done!"
	@rm -rf packed/$(name)_demo
	
	$(GET_TIME)
	
	@echo "[--- END ---]"
	@echo 
	
	$(WARNABOUT7ZIP)


pack-release:
	$(WARNABOUT7ZIP)

	@echo ""
	@echo "[--- START ---]"

	$(GET_TIME)
	
	@echo "Packing files for '$@'..."
	@echo "[0/3] Creating folders..."
	@mkdir -p packed
	@mkdir -p packed/$(name)
	
	@echo "[1/3] Copying contents..."
	@cp -r -v $(RElEASE_DATA) packed/$(name)
	@cp -r -v res/shaders/ packed/$(name)
	@cp -r -v res/subsys/ packed/$(name)
	@cp -r -v res/*.dll packed/$(name)
	@cp -r -v res/$(name).exe packed/$(name)
	@cp -r -v $(extra-progs) packed/$(name)
	@cp -r -v $(extra-files)/* packed/$(name)
	
	@echo "[2/3] Zipping files..."
	
	@cd packed
	
	@cd $(name)
	$(DATA_ARCHIVE)
	$(DATA_REMOVE)
	@cd ..
	
	$(ZIPCMD) $(name).zip $(name)
	
	@cd ..
	
	@echo "[3/3] Done!"
	@rm -rf packed/$(name)
	
	$(GET_TIME)
	
	@echo "[--- END ---]"
	@echo 
	
	$(WARNABOUT7ZIP)


pack-test: pack-debug pack-release


pack-preview: pack-debug pack-demo


pack-both: pack-demo pack-release


pack-all: pack-debug pack-demo pack-release

clean:
	@echo "Cleaning up..."
	@rm -rf $(CLEAN_TARGET)
	@echo "Done!"
