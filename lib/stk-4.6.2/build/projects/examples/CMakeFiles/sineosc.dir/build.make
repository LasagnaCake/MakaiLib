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
include projects/examples/CMakeFiles/sineosc.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include projects/examples/CMakeFiles/sineosc.dir/compiler_depend.make

# Include the progress variables for this target.
include projects/examples/CMakeFiles/sineosc.dir/progress.make

# Include the compile flags for this target's objects.
include projects/examples/CMakeFiles/sineosc.dir/flags.make

projects/examples/CMakeFiles/sineosc.dir/sineosc.cpp.obj: projects/examples/CMakeFiles/sineosc.dir/flags.make
projects/examples/CMakeFiles/sineosc.dir/sineosc.cpp.obj: projects/examples/CMakeFiles/sineosc.dir/includes_CXX.rsp
projects/examples/CMakeFiles/sineosc.dir/sineosc.cpp.obj: ../projects/examples/sineosc.cpp
projects/examples/CMakeFiles/sineosc.dir/sineosc.cpp.obj: projects/examples/CMakeFiles/sineosc.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object projects/examples/CMakeFiles/sineosc.dir/sineosc.cpp.obj"
	cd /d C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\examples && D:\TDM-GCC-64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT projects/examples/CMakeFiles/sineosc.dir/sineosc.cpp.obj -MF CMakeFiles\sineosc.dir\sineosc.cpp.obj.d -o CMakeFiles\sineosc.dir\sineosc.cpp.obj -c C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\projects\examples\sineosc.cpp

projects/examples/CMakeFiles/sineosc.dir/sineosc.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/sineosc.dir/sineosc.cpp.i"
	cd /d C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\examples && D:\TDM-GCC-64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\projects\examples\sineosc.cpp > CMakeFiles\sineosc.dir\sineosc.cpp.i

projects/examples/CMakeFiles/sineosc.dir/sineosc.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/sineosc.dir/sineosc.cpp.s"
	cd /d C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\examples && D:\TDM-GCC-64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\projects\examples\sineosc.cpp -o CMakeFiles\sineosc.dir\sineosc.cpp.s

# Object files for target sineosc
sineosc_OBJECTS = \
"CMakeFiles/sineosc.dir/sineosc.cpp.obj"

# External object files for target sineosc
sineosc_EXTERNAL_OBJECTS =

../projects/examples/sineosc.exe: projects/examples/CMakeFiles/sineosc.dir/sineosc.cpp.obj
../projects/examples/sineosc.exe: projects/examples/CMakeFiles/sineosc.dir/build.make
../projects/examples/sineosc.exe: libstk.a
../projects/examples/sineosc.exe: projects/examples/CMakeFiles/sineosc.dir/linklibs.rsp
../projects/examples/sineosc.exe: projects/examples/CMakeFiles/sineosc.dir/objects1.rsp
../projects/examples/sineosc.exe: projects/examples/CMakeFiles/sineosc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ..\..\..\projects\examples\sineosc.exe"
	cd /d C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\examples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\sineosc.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
projects/examples/CMakeFiles/sineosc.dir/build: ../projects/examples/sineosc.exe
.PHONY : projects/examples/CMakeFiles/sineosc.dir/build

projects/examples/CMakeFiles/sineosc.dir/clean:
	cd /d C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\examples && $(CMAKE_COMMAND) -P CMakeFiles\sineosc.dir\cmake_clean.cmake
.PHONY : projects/examples/CMakeFiles/sineosc.dir/clean

projects/examples/CMakeFiles/sineosc.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2 C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\projects\examples C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\examples C:\Users\patri\Downloads\stk-4.6.2\stk-4.6.2\build\projects\examples\CMakeFiles\sineosc.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : projects/examples/CMakeFiles/sineosc.dir/depend

