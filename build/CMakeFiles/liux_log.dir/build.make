# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Produce verbose output by default.
VERBOSE = 1

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
CMAKE_SOURCE_DIR = /root/liux_simple_server

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/liux_simple_server/build

# Include any dependencies generated for this target.
include CMakeFiles/liux_log.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/liux_log.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/liux_log.dir/flags.make

CMakeFiles/liux_log.dir/src/log.cpp.o: CMakeFiles/liux_log.dir/flags.make
CMakeFiles/liux_log.dir/src/log.cpp.o: ../src/log.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/liux_simple_server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/liux_log.dir/src/log.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/liux_log.dir/src/log.cpp.o -c /root/liux_simple_server/src/log.cpp

CMakeFiles/liux_log.dir/src/log.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/liux_log.dir/src/log.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/liux_simple_server/src/log.cpp > CMakeFiles/liux_log.dir/src/log.cpp.i

CMakeFiles/liux_log.dir/src/log.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/liux_log.dir/src/log.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/liux_simple_server/src/log.cpp -o CMakeFiles/liux_log.dir/src/log.cpp.s

CMakeFiles/liux_log.dir/src/log.cpp.o.requires:

.PHONY : CMakeFiles/liux_log.dir/src/log.cpp.o.requires

CMakeFiles/liux_log.dir/src/log.cpp.o.provides: CMakeFiles/liux_log.dir/src/log.cpp.o.requires
	$(MAKE) -f CMakeFiles/liux_log.dir/build.make CMakeFiles/liux_log.dir/src/log.cpp.o.provides.build
.PHONY : CMakeFiles/liux_log.dir/src/log.cpp.o.provides

CMakeFiles/liux_log.dir/src/log.cpp.o.provides.build: CMakeFiles/liux_log.dir/src/log.cpp.o


# Object files for target liux_log
liux_log_OBJECTS = \
"CMakeFiles/liux_log.dir/src/log.cpp.o"

# External object files for target liux_log
liux_log_EXTERNAL_OBJECTS =

../lib/libliux_log.so: CMakeFiles/liux_log.dir/src/log.cpp.o
../lib/libliux_log.so: CMakeFiles/liux_log.dir/build.make
../lib/libliux_log.so: CMakeFiles/liux_log.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/liux_simple_server/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared library ../lib/libliux_log.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/liux_log.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/liux_log.dir/build: ../lib/libliux_log.so

.PHONY : CMakeFiles/liux_log.dir/build

CMakeFiles/liux_log.dir/requires: CMakeFiles/liux_log.dir/src/log.cpp.o.requires

.PHONY : CMakeFiles/liux_log.dir/requires

CMakeFiles/liux_log.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/liux_log.dir/cmake_clean.cmake
.PHONY : CMakeFiles/liux_log.dir/clean

CMakeFiles/liux_log.dir/depend:
	cd /root/liux_simple_server/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/liux_simple_server /root/liux_simple_server /root/liux_simple_server/build /root/liux_simple_server/build /root/liux_simple_server/build/CMakeFiles/liux_log.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/liux_log.dir/depend

