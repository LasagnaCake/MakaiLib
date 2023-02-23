# Makai Game Engine

## Overview

This is (currently) a header-only C++20 bullet hell engine, that utilizes SDL as a rendering and audio backend.

Currently in active development.

## Required Libraries

None. All necessary components are (presumably) already within the ```lib/``` folder.

## How To Use

1) Create a C++ source file in the root directory (alongside the make file);
2) Add ```#include "res/makai.hpp"``` in your source file;
3) Program your game (TODO: Tutorial and Documentation);
4) Open directory on command line (Or MSYS, if you're using it);
5) Compile with ```make [debug|release] target=YOUR_FILE_NAME.cpp name=YOUR_PROGRAM_NAME```.

Note: the ```target``` only needs to be set if your file is not named ```main.cpp```.

## Documentation

Currently nonexistent. Some of the code is commented.
