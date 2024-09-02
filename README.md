# Makai Game Engine

[![Core Library CI](https://github.com/LasagnaCake/MakaiLib/actions/workflows/core-lib.yml/badge.svg)](https://github.com/LasagnaCake/MakaiLib/actions/workflows/core-lib.yml)

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

## Overview

This is (currently) a header-only C++20 bullet hell engine, that utilizes SDL2 as a rendering and audio backend.

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
| SDL2       | Window & Input handling                             |
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


See [TODO](docs/changes/TODO.md)
