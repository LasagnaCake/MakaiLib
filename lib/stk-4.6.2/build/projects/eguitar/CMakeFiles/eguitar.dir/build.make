# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = D:\CMake\bin\cmake.exe

# The command to remove a file.
RM = D:\CMake\bin\cmake.exe -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build

# Include any dependencies generated for this target.
include projects/eguitar/CMakeFiles/eguitar.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include projects/eguitar/CMakeFiles/eguitar.dir/compiler_depend.make

# Include the progress variables for this target.
include projects/eguitar/CMakeFiles/eguitar.dir/progress.make

# Include the compile flags for this target's objects.
include projects/eguitar/CMakeFiles/eguitar.dir/flags.make

projects/eguitar/CMakeFiles/eguitar.dir/eguitar.cpp.obj: projects/eguitar/CMakeFiles/eguitar.dir/flags.make
projects/eguitar/CMakeFiles/eguitar.dir/eguitar.cpp.obj: projects/eguitar/CMakeFiles/eguitar.dir/includes_CXX.rsp
projects/eguitar/CMakeFiles/eguitar.dir/eguitar.cpp.obj: ../projects/eguitar/eguitar.cpp
projects/eguitar/CMakeFiles/eguitar.dir/eguitar.cpp.obj: projects/eguitar/CMakeFiles/eguitar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object projects/eguitar/CMakeFiles/eguitar.dir/eguitar.cpp.obj"
	cd /d C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\eguitar && D:\TDM-GCC-64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT projects/eguitar/CMakeFiles/eguitar.dir/eguitar.cpp.obj -MF CMakeFiles\eguitar.dir\eguitar.cpp.obj.d -o CMakeFiles\eguitar.dir\eguitar.cpp.obj -c C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\projects\eguitar\eguitar.cpp

projects/eguitar/CMakeFiles/eguitar.dir/eguitar.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/eguitar.dir/eguitar.cpp.i"
	cd /d C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\eguitar && D:\TDM-GCC-64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\projects\eguitar\eguitar.cpp > CMakeFiles\eguitar.dir\eguitar.cpp.i

projects/eguitar/CMakeFiles/eguitar.dir/eguitar.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/eguitar.dir/eguitar.cpp.s"
	cd /d C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\eguitar && D:\TDM-GCC-64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\projects\eguitar\eguitar.cpp -o CMakeFiles\eguitar.dir\eguitar.cpp.s

projects/eguitar/CMakeFiles/eguitar.dir/utilities.cpp.obj: projects/eguitar/CMakeFiles/eguitar.dir/flags.make
projects/eguitar/CMakeFiles/eguitar.dir/utilities.cpp.obj: projects/eguitar/CMakeFiles/eguitar.dir/includes_CXX.rsp
projects/eguitar/CMakeFiles/eguitar.dir/utilities.cpp.obj: ../projects/eguitar/utilities.cpp
projects/eguitar/CMakeFiles/eguitar.dir/utilities.cpp.obj: projects/eguitar/CMakeFiles/eguitar.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object projects/eguitar/CMakeFiles/eguitar.dir/utilities.cpp.obj"
	cd /d C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\eguitar && D:\TDM-GCC-64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT projects/eguitar/CMakeFiles/eguitar.dir/utilities.cpp.obj -MF CMakeFiles\eguitar.dir\utilities.cpp.obj.d -o CMakeFiles\eguitar.dir\utilities.cpp.obj -c C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\projects\eguitar\utilities.cpp

projects/eguitar/CMakeFiles/eguitar.dir/utilities.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/eguitar.dir/utilities.cpp.i"
	cd /d C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\eguitar && D:\TDM-GCC-64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\projects\eguitar\utilities.cpp > CMakeFiles\eguitar.dir\utilities.cpp.i

projects/eguitar/CMakeFiles/eguitar.dir/utilities.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/eguitar.dir/utilities.cpp.s"
	cd /d C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\eguitar && D:\TDM-GCC-64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\projects\eguitar\utilities.cpp -o CMakeFiles\eguitar.dir\utilities.cpp.s

# Object files for target eguitar
eguitar_OBJECTS = \
"CMakeFiles/eguitar.dir/eguitar.cpp.obj" \
"CMakeFiles/eguitar.dir/utilities.cpp.obj"

# External object files for target eguitar
eguitar_EXTERNAL_OBJECTS =

../projects/eguitar/eguitar.exe: projects/eguitar/CMakeFiles/eguitar.dir/eguitar.cpp.obj
../projects/eguitar/eguitar.exe: projects/eguitar/CMakeFiles/eguitar.dir/utilities.cpp.obj
../projects/eguitar/eguitar.exe: projects/eguitar/CMakeFiles/eguitar.dir/build.make
../projects/eguitar/eguitar.exe: libstk.a
../projects/eguitar/eguitar.exe: projects/eguitar/CMakeFiles/eguitar.dir/linklibs.rsp
../projects/eguitar/eguitar.exe: projects/eguitar/CMakeFiles/eguitar.dir/objects1.rsp
../projects/eguitar/eguitar.exe: projects/eguitar/CMakeFiles/eguitar.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable ..\..\..\projects\eguitar\eguitar.exe"
	cd /d C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\eguitar && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\eguitar.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
projects/eguitar/CMakeFiles/eguitar.dir/build: ../projects/eguitar/eguitar.exe
.PHONY : projects/eguitar/CMakeFiles/eguitar.dir/build

projects/eguitar/CMakeFiles/eguitar.dir/clean:
	cd /d C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\eguitar && $(CMAKE_COMMAND) -P CMakeFiles\eguitar.dir\cmake_clean.cmake
.PHONY : projects/eguitar/CMakeFiles/eguitar.dir/clean

projects/eguitar/CMakeFiles/eguitar.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2 C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\projects\eguitar C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\eguitar C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\eguitar\CMakeFiles\eguitar.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : projects/eguitar/CMakeFiles/eguitar.dir/depend

