# Makai Game Engine

## Table of contents

- [Overview](#Overview)
- [Supported Operating Systems](#Supported-Operating-Systems)
- [Requirements](#Requirements)
- - [Windows](#Windows)
- [How to Use](#How-to-Use)
- - [Note](#Note)
- [Libraries Used](#Libraries-Used)
- [Documentation](#Documentation)
- [TODO](#TODO)
- - [Future](#Future)

## Overview

This is (currently) a header-only C++20 bullet hell engine, that utilizes SDL as a rendering and audio backend.

Currently in active development.

Enough spaghetti (code) to put an italian restaurant out of business.

## Supported operating systems

- 64-bit Windows

No plans to support 32-bit systems.

## Requirements

### Windows

- GCC via MinGW (MSYS (Recommended), Cygwin, TDM-GCC) (DOES NOT SUPPORT CLANG)

## How To use

1) Create a C++ source file within the ```build/``` directory (create directory if nonexistent);
2) Add ```#include <makai.hpp>``` in your source file;
3) Program your game <mark>(TODO: Tutorial and Documentation)</mark>;
4) Open root directory on command line (MSYS MinGW64 or Cygwin, if you're using it);
5) Compile with ```make (TARGET) [src=PATH_TO_YOUR_FILE.cpp [name=YOUR_PROGRAM_NAME]]``` (See [notes](#Note) for more detail);
6) Done! Your compiled program should be within the ```res/``` directory.

### Note

For more information, type ```make help```.

The ```src``` only needs to be set if your file is not at the top of the ```build/``` folder or not named ```main.cpp```.

## Libraries used

These are already included in the ```lib/``` folder.

| Name       | Purpose                  |
|:----------:|--------------------------|
| SDL        | Window & Input handling  |
| OpenGL     | Graphics backend         |
| SDL2_Mixer | Audio backend            |
| stb_image  | Image loading            |
| cute_c2    | Collision                |
| jsoncpp    | Multiple things          |
| cppcodec   | Data encoding & decoding |

## Documentation

Currently VERY scarce. Some of the code is commented.

## TODO

### Current

- [ ] Optimize GPU usage
- [x] Replace `.ini`-based savefile system with a different format
- - Using JSON
- [x] GUI system
- [x] Menu system
- [x] Dialogue system
- [ ] Spellcard system
- [ ] Curvy/Bent lasers
- [ ] Bugs & Errors (see [Issues.txt](Issues.txt))
- [ ] File format implementation (These need better names):
- - For more info, see the [specifications](docs/specifications) folder
- - [ ] Material configuration definitions
- - - [x] ObjectMaterial
- - - [ ] WorldMaterial
- - - [ ] BufferMaterial
- - [x] Renderable object definition
- - - [x] Mesh Parser
- - - [x] Transform Parser
- - - [x] Material Parser
- - - - Requires ObjectMaterial definition
- - [ ] Animation keyframes definition
- - [ ] Danmaku stage layout definition
- - - Possibly via combining 2nd & 3rd implementations
- [ ] Custom language (compiled (preferable) or interpreted)
- - Currently have an idea, don't know if it would be feasible
- - Could also implement support for Squirrel/LUA/AngelScript

### Future

- [ ] Other game types
- [ ] More "multi-purpose" game classes
- [ ] Cleanup & polish [entity](src/collection/entity/entity.hpp) system
- [ ] Multi-platform support:
- - [ ] Linux
- - [ ] Mac

#### Major refactor

- [ ] "De-headerify" library
- [ ] Cleanup & polish [graphical](src/graphical) system
- [ ] Cleanup & polish [audio](src/audio) system
- [ ] Replace raw pointers with smart pointers (see [referential.hpp](src/collection/referential.hpp))
- [x] Remove or replace `$` macros
- [ ] Reorganize and remove haphazardness/mess
- [ ] "C++20 coroutine"-ify [tasking system](src/collection/tasking.hpp)

#### Under consideration

- C++20 modules?
- Rename stuff

### Very far future

- Vulkanize graphical system (VERY DIFFICULT, might not be done)
