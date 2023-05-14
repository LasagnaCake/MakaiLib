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

| Name | Purpose |
|:--:|:---|
| SDL2 | Graphical rendering |
| SDL2_Mixer | Audio backend |
| GLM | (some) 3D Math |
| stb_image | Image loading |
| cute_c2 | Collision |
| mINI (TO BE REPLACED) | Savefile handling |
| jsoncpp | Multiple things |
| cppcodec | Multiple things |

## Documentation

Currently nonexistent. Some of the code is commented.

## TODO

### Current

- [ ] Replace `.ini`-based savefile system with a different format
- - Either TOML, JSON or YAML
- [ ] GUI system
- [ ] Menu system
- [ ] Spellcard system
- [ ] Bugs & Errors (see [Issues.txt](Issues.txt))
- [ ] File format implementation (These need better names):
- - For more info, see the [specifications](docs/specifications) folder
- - [ ] Material configuration definitions
- - - [ ] ObjectMaterial
- - - [ ] WorldMaterial
- - - [ ] BufferMaterial
- - [ ] Renderable object definition
- - - [x] Mesh Parser
- - - [x] Transform Parser
- - - [ ] Material Parser
- - - - Requires ObjectMaterial definition
- - [ ] Animation keyframes definition
- - [ ] Danmaku stage layout definition
- - - Possibly via combining 1st & 2nd implementations
- [ ] Custom language (compiled (preferable) or interpreted)
- - Currently have an idea, don't know if it would be feasible
- - Could also implement support for Squirrel/LUA

### Future

- [ ] Other game types
- [ ] More "multi-purpose" game classes
- [ ] Multi-platform support:
- - [ ] Linux
- - [ ] Mac

#### Major refactor

- [ ] "De-headerify" library
- [ ] Cleanup & polish [vectorn.hpp](src/collection/vectorn.hpp) (Potentially turn it into an "universal vector" module)
- [ ] Cleanup & polish [algebra.hpp](src/collection/algebra.hpp)
- - [ ] "Template"-ify functions where applicable
- [ ] Cleanup & polish [graphical](src/graphical) system
- [ ] Cleanup & polish [audio](src/audio) system
- [ ] Replace raw pointers with smart pointers (see [referential.hpp](src/collection/referential.hpp))
- [ ] Remove or replace `$` macros
- [ ] Reorganize and remove haphazardness/mess
- [ ] Shorten namespace names or unify them under the `Makai` namespace
- [ ] "C++20 coroutine"-ify [tasking system](src/collection/tasking.hpp)

#### Under consideration

- C++20 modules?
- Replace or refactor [entity](src/collection/entity) system
- Rename stuff

### Very far future

- Vulkanize graphical system (VERY DIFFICULT)
