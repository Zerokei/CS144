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
CMAKE_SOURCE_DIR = /home/onekei/sponge

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/onekei/sponge

# Include any dependencies generated for this target.
include apps/CMakeFiles/lab4.dir/depend.make

# Include the progress variables for this target.
include apps/CMakeFiles/lab4.dir/progress.make

# Include the compile flags for this target's objects.
include apps/CMakeFiles/lab4.dir/flags.make

apps/CMakeFiles/lab4.dir/lab4.cc.o: apps/CMakeFiles/lab4.dir/flags.make
apps/CMakeFiles/lab4.dir/lab4.cc.o: apps/lab4.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/onekei/sponge/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object apps/CMakeFiles/lab4.dir/lab4.cc.o"
	cd /home/onekei/sponge/apps && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/lab4.dir/lab4.cc.o -c /home/onekei/sponge/apps/lab4.cc

apps/CMakeFiles/lab4.dir/lab4.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/lab4.dir/lab4.cc.i"
	cd /home/onekei/sponge/apps && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/onekei/sponge/apps/lab4.cc > CMakeFiles/lab4.dir/lab4.cc.i

apps/CMakeFiles/lab4.dir/lab4.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/lab4.dir/lab4.cc.s"
	cd /home/onekei/sponge/apps && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/onekei/sponge/apps/lab4.cc -o CMakeFiles/lab4.dir/lab4.cc.s

# Object files for target lab4
lab4_OBJECTS = \
"CMakeFiles/lab4.dir/lab4.cc.o"

# External object files for target lab4
lab4_EXTERNAL_OBJECTS =

apps/lab4: apps/CMakeFiles/lab4.dir/lab4.cc.o
apps/lab4: apps/CMakeFiles/lab4.dir/build.make
apps/lab4: apps/libstream_copy.a
apps/lab4: libsponge/libsponge.a
apps/lab4: /usr/lib/x86_64-linux-gnu/libpthread.so
apps/lab4: apps/CMakeFiles/lab4.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/onekei/sponge/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable lab4"
	cd /home/onekei/sponge/apps && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lab4.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
apps/CMakeFiles/lab4.dir/build: apps/lab4

.PHONY : apps/CMakeFiles/lab4.dir/build

apps/CMakeFiles/lab4.dir/clean:
	cd /home/onekei/sponge/apps && $(CMAKE_COMMAND) -P CMakeFiles/lab4.dir/cmake_clean.cmake
.PHONY : apps/CMakeFiles/lab4.dir/clean

apps/CMakeFiles/lab4.dir/depend:
	cd /home/onekei/sponge && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/onekei/sponge /home/onekei/sponge/apps /home/onekei/sponge /home/onekei/sponge/apps /home/onekei/sponge/apps/CMakeFiles/lab4.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : apps/CMakeFiles/lab4.dir/depend

