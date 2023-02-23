# Makai Game Engine

## Overview

This is (currently) a header-only C++20 bullet hell engine, that utilizes SDL as a rendering and audio backend.

Currently in active development.

## Required Libraries

None. All necessary components are (presumably) already within the ```lib/``` folder.

## How To Use

1) Create a C++ source file within the ```build/``` directory;
2) Add ```#include "res/makai.hpp"``` in your source file;
3) Program your game <mark>(TODO: Tutorial and Documentation)</mark>;
4) Open root directory on command line (Or MSYS, if you're using it);
5) Compile with ```make [debug|release] target=PATH_TO_YOUR_FILE.cpp name=YOUR_PROGRAM_NAME```;
6) Done! Your compiled program should be within the ```res/``` directory.

Note: the ```target``` only needs to be set if your file is not named ```main.cpp```.

## Documentation

Currently nonexistent. Some of the code is commented.
