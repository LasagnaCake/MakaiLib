# Makai 3D Application Engine

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

Formerly a C++20 header-only game engine.

Now, it's a C++20 3D application engine, built on top of SDL2 & OpenGL.
The game engine part is getting refactored into a separate extension.

Now upgraded from spaghetti code to penne rigattoni code.

## Supported operating systems

- 64-bit Windows

No plans to support 32-bit systems.

## Requirements

### Windows

**WARNING: This library does not support clang/LLVM!**

- GCC via MinGW (MSYS (Recommended), Cygwin, TDM-GCC) 
- Make

If using MSYS, all of them must be installed via pacman:

```
For toolchain (GCC): pacman -S mingw-w64-x86_64-toolchain
For make: pacman -S make

In a single line: pacman -S mingw-w64-x86_64-toolchain make
```

### GCC toolchain version

<mark>Your GCC toolchain **must be version 13, preferably 13.2.0-2!**</mark> To ensure that it is the correct version, run the following command:

```
pacman -U -d https://repo.msys2.org/mingw/mingw64/mingw-w64-x86_64-gcc-libs-13.2.0-2-any.pkg.tar.zst ; pacman -U -d https://repo.msys2.org/mingw/mingw64/mingw-w64-x86_64-gcc-13.2.0-2-any.pkg.tar.zst
```

For more information as to why, see [Issues.txt](Issues.txt).

## How to use

For the old system, se the `legacy-system` branch.

### Building from scratch

1. Clone the repository (or just download it);
2. Enter the repository via command line (whichever has GCC);
3. Run `make all`;
4. Done! The result is located in the generated `output/` folder.

### Note

## Libraries used

Inclusion of these on your project are not required, and **strictly forbidden** (except OpenGL, which is **required**).
Since a version of (most of) them are bundled[^1], including your own version of those *will* cause issues.

| Name       | Purpose                                             | Bundled? |
|:----------:|-----------------------------------------------------|:-:|
| SDL2       | Window & Input handling                             | Yes (Lib file) |
| OpenGL     | Graphics backend                                    | No |
| SDL2_Mixer | Audio backend                                       | Yes (Lib file) |
| SDL2_Net   | Networking Backend                                  | Yes (Lib file) |
| stb_image  | Image loading                                       | Yes (Lib file) |
| cute_c2    | Collision                                           | Yes (Header-only, modified) |
| nlohmann::jsoncpp   | Multiple things                                     | Yes (Header-only) |
| cppcodec   | Data encoding & decoding                            | Only used internally, not required |
| CryptoPP   | Encryption, decryption, compression & decompression | Yes (Lib file) |

## Documentation

Currently VERY scarce. Some of the code is commented.

## TODO

See [TODO](docs/changes/TODO.md)

[^1]: All libraries that have a library (`*.a`) file are appended into the library file (`libmakai.debug.a` and `libmakai.a`).
`stb_image`'s implementation is also appended.
`nlohmann::jsoncpp` & `cute_c2` (modified version) are header-only, thus bundled in the headers.
