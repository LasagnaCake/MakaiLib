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
include projects/examples/CMakeFiles/record.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include projects/examples/CMakeFiles/record.dir/compiler_depend.make

# Include the progress variables for this target.
include projects/examples/CMakeFiles/record.dir/progress.make

# Include the compile flags for this target's objects.
include projects/examples/CMakeFiles/record.dir/flags.make

projects/examples/CMakeFiles/record.dir/record.cpp.obj: projects/examples/CMakeFiles/record.dir/flags.make
projects/examples/CMakeFiles/record.dir/record.cpp.obj: projects/examples/CMakeFiles/record.dir/includes_CXX.rsp
projects/examples/CMakeFiles/record.dir/record.cpp.obj: ../projects/examples/record.cpp
projects/examples/CMakeFiles/record.dir/record.cpp.obj: projects/examples/CMakeFiles/record.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object projects/examples/CMakeFiles/record.dir/record.cpp.obj"
	cd /d C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\examples && D:\TDM-GCC-64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT projects/examples/CMakeFiles/record.dir/record.cpp.obj -MF CMakeFiles\record.dir\record.cpp.obj.d -o CMakeFiles\record.dir\record.cpp.obj -c C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\projects\examples\record.cpp

projects/examples/CMakeFiles/record.dir/record.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/record.dir/record.cpp.i"
	cd /d C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\examples && D:\TDM-GCC-64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\projects\examples\record.cpp > CMakeFiles\record.dir\record.cpp.i

projects/examples/CMakeFiles/record.dir/record.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/record.dir/record.cpp.s"
	cd /d C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\examples && D:\TDM-GCC-64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\projects\examples\record.cpp -o CMakeFiles\record.dir\record.cpp.s

# Object files for target record
record_OBJECTS = \
"CMakeFiles/record.dir/record.cpp.obj"

# External object files for target record
record_EXTERNAL_OBJECTS =

../projects/examples/record.exe: projects/examples/CMakeFiles/record.dir/record.cpp.obj
../projects/examples/record.exe: projects/examples/CMakeFiles/record.dir/build.make
../projects/examples/record.exe: libstk.a
../projects/examples/record.exe: projects/examples/CMakeFiles/record.dir/linklibs.rsp
../projects/examples/record.exe: projects/examples/CMakeFiles/record.dir/objects1.rsp
../projects/examples/record.exe: projects/examples/CMakeFiles/record.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ..\..\..\projects\examples\record.exe"
	cd /d C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\record.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
projects/examples/CMakeFiles/record.dir/build: ../projects/examples/record.exe
.PHONY : projects/examples/CMakeFiles/record.dir/build

projects/examples/CMakeFiles/record.dir/clean:
	cd /d C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\examples && $(CMAKE_COMMAND) -P CMakeFiles\record.dir\cmake_clean.cmake
.PHONY : projects/examples/CMakeFiles/record.dir/clean

projects/examples/CMakeFiles/record.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2 C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\projects\examples C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\examples C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\examples\CMakeFiles\record.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : projects/examples/CMakeFiles/record.dir/depend

