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
include tests/CMakeFiles/send_extra.dir/depend.make

# Include the progress variables for this target.
include tests/CMakeFiles/send_extra.dir/progress.make

# Include the compile flags for this target's objects.
include tests/CMakeFiles/send_extra.dir/flags.make

tests/CMakeFiles/send_extra.dir/send_extra.cc.o: tests/CMakeFiles/send_extra.dir/flags.make
tests/CMakeFiles/send_extra.dir/send_extra.cc.o: tests/send_extra.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/onekei/sponge/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object tests/CMakeFiles/send_extra.dir/send_extra.cc.o"
	cd /home/onekei/sponge/tests && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/send_extra.dir/send_extra.cc.o -c /home/onekei/sponge/tests/send_extra.cc

tests/CMakeFiles/send_extra.dir/send_extra.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/send_extra.dir/send_extra.cc.i"
	cd /home/onekei/sponge/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/onekei/sponge/tests/send_extra.cc > CMakeFiles/send_extra.dir/send_extra.cc.i

tests/CMakeFiles/send_extra.dir/send_extra.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/send_extra.dir/send_extra.cc.s"
	cd /home/onekei/sponge/tests && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/onekei/sponge/tests/send_extra.cc -o CMakeFiles/send_extra.dir/send_extra.cc.s

# Object files for target send_extra
send_extra_OBJECTS = \
"CMakeFiles/send_extra.dir/send_extra.cc.o"

# External object files for target send_extra
send_extra_EXTERNAL_OBJECTS =

tests/send_extra: tests/CMakeFiles/send_extra.dir/send_extra.cc.o
tests/send_extra: tests/CMakeFiles/send_extra.dir/build.make
tests/send_extra: tests/libspongechecks.a
tests/send_extra: libsponge/libsponge.a
tests/send_extra: tests/CMakeFiles/send_extra.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/onekei/sponge/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable send_extra"
	cd /home/onekei/sponge/tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/send_extra.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
tests/CMakeFiles/send_extra.dir/build: tests/send_extra

.PHONY : tests/CMakeFiles/send_extra.dir/build

tests/CMakeFiles/send_extra.dir/clean:
	cd /home/onekei/sponge/tests && $(CMAKE_COMMAND) -P CMakeFiles/send_extra.dir/cmake_clean.cmake
.PHONY : tests/CMakeFiles/send_extra.dir/clean

tests/CMakeFiles/send_extra.dir/depend:
	cd /home/onekei/sponge && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/onekei/sponge /home/onekei/sponge/tests /home/onekei/sponge /home/onekei/sponge/tests /home/onekei/sponge/tests/CMakeFiles/send_extra.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : tests/CMakeFiles/send_extra.dir/depend

