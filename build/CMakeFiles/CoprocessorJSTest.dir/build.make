# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/camh/Documents/Retro68kApps/CoprocessorJSTest

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/camh/Documents/Retro68kApps/CoprocessorJSTest/build

# Include any dependencies generated for this target.
include CMakeFiles/CoprocessorJSTest.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/CoprocessorJSTest.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/CoprocessorJSTest.dir/flags.make

CMakeFiles/CoprocessorJSTest.dir/hello.c.obj: CMakeFiles/CoprocessorJSTest.dir/flags.make
CMakeFiles/CoprocessorJSTest.dir/hello.c.obj: ../hello.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/camh/Documents/Retro68kApps/CoprocessorJSTest/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/CoprocessorJSTest.dir/hello.c.obj"
	/home/camh/Retro68-build/toolchain/bin/m68k-apple-macos-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/CoprocessorJSTest.dir/hello.c.obj   -c /home/camh/Documents/Retro68kApps/CoprocessorJSTest/hello.c

CMakeFiles/CoprocessorJSTest.dir/hello.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/CoprocessorJSTest.dir/hello.c.i"
	/home/camh/Retro68-build/toolchain/bin/m68k-apple-macos-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/camh/Documents/Retro68kApps/CoprocessorJSTest/hello.c > CMakeFiles/CoprocessorJSTest.dir/hello.c.i

CMakeFiles/CoprocessorJSTest.dir/hello.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/CoprocessorJSTest.dir/hello.c.s"
	/home/camh/Retro68-build/toolchain/bin/m68k-apple-macos-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/camh/Documents/Retro68kApps/CoprocessorJSTest/hello.c -o CMakeFiles/CoprocessorJSTest.dir/hello.c.s

# Object files for target CoprocessorJSTest
CoprocessorJSTest_OBJECTS = \
"CMakeFiles/CoprocessorJSTest.dir/hello.c.obj"

# External object files for target CoprocessorJSTest
CoprocessorJSTest_EXTERNAL_OBJECTS =

CoprocessorJSTest.code.bin: CMakeFiles/CoprocessorJSTest.dir/hello.c.obj
CoprocessorJSTest.code.bin: CMakeFiles/CoprocessorJSTest.dir/build.make
CoprocessorJSTest.code.bin: CMakeFiles/CoprocessorJSTest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/camh/Documents/Retro68kApps/CoprocessorJSTest/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable CoprocessorJSTest.code.bin"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/CoprocessorJSTest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/CoprocessorJSTest.dir/build: CoprocessorJSTest.code.bin

.PHONY : CMakeFiles/CoprocessorJSTest.dir/build

CMakeFiles/CoprocessorJSTest.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/CoprocessorJSTest.dir/cmake_clean.cmake
.PHONY : CMakeFiles/CoprocessorJSTest.dir/clean

CMakeFiles/CoprocessorJSTest.dir/depend:
	cd /home/camh/Documents/Retro68kApps/CoprocessorJSTest/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/camh/Documents/Retro68kApps/CoprocessorJSTest /home/camh/Documents/Retro68kApps/CoprocessorJSTest /home/camh/Documents/Retro68kApps/CoprocessorJSTest/build /home/camh/Documents/Retro68kApps/CoprocessorJSTest/build /home/camh/Documents/Retro68kApps/CoprocessorJSTest/build/CMakeFiles/CoprocessorJSTest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/CoprocessorJSTest.dir/depend
