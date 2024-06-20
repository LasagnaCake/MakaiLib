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
- Make
- 7-Zip command-line (Recommended) or zip command-line
- RSync

If using MSYS, all of them must be installed via pacman:

```
For toolchain (GCC): pacman -S mingw-w64-x86_64-toolchain
For make: pacman -S make
For zip: pacman -S zip
For 7-zip: pacman -S p7zip
For rsync: pacman -S rsync

In a single line: pacman -S mingw-w64-x86_64-toolchain zip p7zip make rsync
```

### GCC toolchain version

<mark>Your GCC toolchain **must be version 13, preferably 13.2.0-2!**</mark> To ensure that it is the correct version, run the following command:

```
pacman -U -d https://repo.msys2.org/mingw/mingw64/mingw-w64-x86_64-gcc-libs-13.2.0-2-any.pkg.tar.zst ; pacman -U -d https://repo.msys2.org/mingw/mingw64/mingw-w64-x86_64-gcc-13.2.0-2-any.pkg.tar.zst

```

For more information as to why, see [Issues.txt](Issues.txt).

## How to use

1. Create a C++ source file within the ```build/``` directory (create directory if nonexistent);
2. Add ```#include <makai.hpp>``` in your source file;
3. Program your game <mark>(TODO: Tutorial and Documentation)</mark>;
4. Open root directory on command line (MSYS MinGW64 or Cygwin, if you're using it);
5. Compile with ```make (TARGET) [src=PATH_TO_YOUR_FILE.cpp [name=YOUR_PROGRAM_NAME]]``` (See [notes](#Note) for more detail);
6. Done! Your compiled program should be within the ```res/``` directory.

### Note

For more information, open root directory on command line, and type ```make help```.

The ```src``` only needs to be set if your file is not at the top of the ```build/``` folder or not named ```main.cpp```.

Any resource files that the game uses MUST be placed inside the ```data/``` folder in the ```res/``` directory. If the folder doesn't exist, create it.

The ```data/``` folder gets archived during the packing procedure, if ```no-archive``` is set to `0` (default) and the target is not ```pack-debug```. This allows the folder to be compressed and encrypted, ensuring security.

Any subsystems that the game uses can be placed inside the ```subsys/``` folder in the ```res/``` directory, for organization purposes. If the folder doesn't exist, create it.

The ```subsys/``` folder does not get archived during the packing procedure, so it is recommended to be used for stuff that **must be loaded unarchived**.

## Libraries used

These are already included in the ```lib/``` folder.

| Name       | Purpose                                             |
|:----------:|-----------------------------------------------------|
| SDL        | Window & Input handling                             |
| OpenGL     | Graphics backend                                    |
| SDL2_Mixer | Audio backend                                       |
| stb_image  | Image loading                                       |
| cute_c2    | Collision                                           |
| nlohmann::jsoncpp   | Multiple things                                     |
| cppcodec   | Data encoding & decoding                            |
| CryptoPP   | Encryption, decryption, compression & decompression |

## Documentation

Currently VERY scarce. Some of the code is commented.

## TODO

### Current
- [x] Separate `itembox` and `grazebox`
- [x] `data/` folder packing system
- [x] "Optimize" GPU usage
- - Instead of rendering every layer from the layerbuffer to the framebuffer, maybe implement sublayers?
- - ( ) A: Group every X layers to be rendered by clearing the depth bit in-between layers
- - - - Would play nicely with the sublayers in gamedata
- - - - Would still be somewhat resource-intensive, but not as much
- - (x) B: Create special functions to tell the system when to render the layerbuffer to the framebuffer
- - - - Would be called in the `onPreLayerDraw` step
- - - - Would also play nicely with the sublayers
- - - - Would also be the fastest, but more complicated on the end-user
- - - - This one would also play nicely without messing the current rendering pipeline too much
- - It is now up to the developer to figure out when to render their own layers, or to just just leave it like the old system
- [ ] Implement a framebuffer push-pop stack system, perhaps?
- - On second thought, this might be a stupid idea. Or not
- [x] Replace `.ini`-based savefile system with a different format
- - Using JSON
- [x] GUI system
- [x] Menu system
- [x] Dialogue system
- [ ] Curvy/Bent lasers
- [ ] Bugs & Errors (see [Issues.txt](Issues.txt))
- [ ] File format implementation (These need better names):
- - For more info, see the [specifications](docs/specifications) folder
- - [ ] Material configuration definitions
- - - [x] ObjectMaterial
- - - [x] WorldMaterial
- - - [ ] BufferMaterial
- - [x] Renderable object definition
- - - [x] Mesh Parser
- - - [x] Transform Parser
- - - [x] Material Parser
- - - - Requires ObjectMaterial definition
- - [ ] Animation keyframes definition
- - [ ] Danmaku stage layout definition
- - - Possibly via combining 2nd & 3rd implementations
- [ ] Embedded language (compiled (preferable, even if to bytecode), JIT or interpreted)
- - Currently have an idea, don't know if it would be feasible
- - Could also implement support for Squirrel/LUA/AngelScript
- - Or C# even, this one might be the more feasible option
- - - This one sounds enticing

### Future

- [ ] Other game types
- [ ] More "multi-purpose" game classes
- [ ] Cleanup & polish [entity](src/collection/entity/entity.hpp) system
- [ ] (Maybe) replace JSON to other specification
- - For the *custom file formats*, that is
- - See [File Format Alternatives](docs/changes/AltFormats.md) for more informaion
- [ ] Multi-platform support:
- - [ ] Linux

#### Major refactor

- [ ] Cleanup & polish [graphical](src/graphical) system
- [ ] Cleanup & polish [audio](src/audio) system
- [ ] Replace raw pointers with smart pointers (see [referential.hpp](src/collection/referential.hpp))
- [x] Remove or replace `$` macros
- [ ] Reorganize and remove haphazardness/mess
- [ ] "Async"-ify [tasking system](src/collection/tasking.hpp) (see [asynchronous.hpp](src/collection/asynchronous.hpp))

#### Under consideration

- C++20 modules?
- Rename stuff

### Very far future

- Vulkanize graphical system (VERY DIFFICULT, might not be done)
