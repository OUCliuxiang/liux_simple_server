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
CMAKE_SOURCE_DIR = /root/worksapce/sylarServer

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/worksapce/sylarServer/build

# Include any dependencies generated for this target.
include CMakeFiles/liux_fiber.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/liux_fiber.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/liux_fiber.dir/flags.make

CMakeFiles/liux_fiber.dir/src/fiber.cpp.o: CMakeFiles/liux_fiber.dir/flags.make
CMakeFiles/liux_fiber.dir/src/fiber.cpp.o: ../src/fiber.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/worksapce/sylarServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/liux_fiber.dir/src/fiber.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/liux_fiber.dir/src/fiber.cpp.o -c /root/worksapce/sylarServer/src/fiber.cpp

CMakeFiles/liux_fiber.dir/src/fiber.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/liux_fiber.dir/src/fiber.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/worksapce/sylarServer/src/fiber.cpp > CMakeFiles/liux_fiber.dir/src/fiber.cpp.i

CMakeFiles/liux_fiber.dir/src/fiber.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/liux_fiber.dir/src/fiber.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/worksapce/sylarServer/src/fiber.cpp -o CMakeFiles/liux_fiber.dir/src/fiber.cpp.s

CMakeFiles/liux_fiber.dir/src/fiber.cpp.o.requires:

.PHONY : CMakeFiles/liux_fiber.dir/src/fiber.cpp.o.requires

CMakeFiles/liux_fiber.dir/src/fiber.cpp.o.provides: CMakeFiles/liux_fiber.dir/src/fiber.cpp.o.requires
	$(MAKE) -f CMakeFiles/liux_fiber.dir/build.make CMakeFiles/liux_fiber.dir/src/fiber.cpp.o.provides.build
.PHONY : CMakeFiles/liux_fiber.dir/src/fiber.cpp.o.provides

CMakeFiles/liux_fiber.dir/src/fiber.cpp.o.provides.build: CMakeFiles/liux_fiber.dir/src/fiber.cpp.o


CMakeFiles/liux_fiber.dir/src/thread.cpp.o: CMakeFiles/liux_fiber.dir/flags.make
CMakeFiles/liux_fiber.dir/src/thread.cpp.o: ../src/thread.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/worksapce/sylarServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/liux_fiber.dir/src/thread.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/liux_fiber.dir/src/thread.cpp.o -c /root/worksapce/sylarServer/src/thread.cpp

CMakeFiles/liux_fiber.dir/src/thread.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/liux_fiber.dir/src/thread.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/worksapce/sylarServer/src/thread.cpp > CMakeFiles/liux_fiber.dir/src/thread.cpp.i

CMakeFiles/liux_fiber.dir/src/thread.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/liux_fiber.dir/src/thread.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/worksapce/sylarServer/src/thread.cpp -o CMakeFiles/liux_fiber.dir/src/thread.cpp.s

CMakeFiles/liux_fiber.dir/src/thread.cpp.o.requires:

.PHONY : CMakeFiles/liux_fiber.dir/src/thread.cpp.o.requires

CMakeFiles/liux_fiber.dir/src/thread.cpp.o.provides: CMakeFiles/liux_fiber.dir/src/thread.cpp.o.requires
	$(MAKE) -f CMakeFiles/liux_fiber.dir/build.make CMakeFiles/liux_fiber.dir/src/thread.cpp.o.provides.build
.PHONY : CMakeFiles/liux_fiber.dir/src/thread.cpp.o.provides

CMakeFiles/liux_fiber.dir/src/thread.cpp.o.provides.build: CMakeFiles/liux_fiber.dir/src/thread.cpp.o


CMakeFiles/liux_fiber.dir/src/config.cpp.o: CMakeFiles/liux_fiber.dir/flags.make
CMakeFiles/liux_fiber.dir/src/config.cpp.o: ../src/config.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/worksapce/sylarServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/liux_fiber.dir/src/config.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/liux_fiber.dir/src/config.cpp.o -c /root/worksapce/sylarServer/src/config.cpp

CMakeFiles/liux_fiber.dir/src/config.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/liux_fiber.dir/src/config.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/worksapce/sylarServer/src/config.cpp > CMakeFiles/liux_fiber.dir/src/config.cpp.i

CMakeFiles/liux_fiber.dir/src/config.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/liux_fiber.dir/src/config.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/worksapce/sylarServer/src/config.cpp -o CMakeFiles/liux_fiber.dir/src/config.cpp.s

CMakeFiles/liux_fiber.dir/src/config.cpp.o.requires:

.PHONY : CMakeFiles/liux_fiber.dir/src/config.cpp.o.requires

CMakeFiles/liux_fiber.dir/src/config.cpp.o.provides: CMakeFiles/liux_fiber.dir/src/config.cpp.o.requires
	$(MAKE) -f CMakeFiles/liux_fiber.dir/build.make CMakeFiles/liux_fiber.dir/src/config.cpp.o.provides.build
.PHONY : CMakeFiles/liux_fiber.dir/src/config.cpp.o.provides

CMakeFiles/liux_fiber.dir/src/config.cpp.o.provides.build: CMakeFiles/liux_fiber.dir/src/config.cpp.o


CMakeFiles/liux_fiber.dir/src/log.cpp.o: CMakeFiles/liux_fiber.dir/flags.make
CMakeFiles/liux_fiber.dir/src/log.cpp.o: ../src/log.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/worksapce/sylarServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/liux_fiber.dir/src/log.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/liux_fiber.dir/src/log.cpp.o -c /root/worksapce/sylarServer/src/log.cpp

CMakeFiles/liux_fiber.dir/src/log.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/liux_fiber.dir/src/log.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/worksapce/sylarServer/src/log.cpp > CMakeFiles/liux_fiber.dir/src/log.cpp.i

CMakeFiles/liux_fiber.dir/src/log.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/liux_fiber.dir/src/log.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/worksapce/sylarServer/src/log.cpp -o CMakeFiles/liux_fiber.dir/src/log.cpp.s

CMakeFiles/liux_fiber.dir/src/log.cpp.o.requires:

.PHONY : CMakeFiles/liux_fiber.dir/src/log.cpp.o.requires

CMakeFiles/liux_fiber.dir/src/log.cpp.o.provides: CMakeFiles/liux_fiber.dir/src/log.cpp.o.requires
	$(MAKE) -f CMakeFiles/liux_fiber.dir/build.make CMakeFiles/liux_fiber.dir/src/log.cpp.o.provides.build
.PHONY : CMakeFiles/liux_fiber.dir/src/log.cpp.o.provides

CMakeFiles/liux_fiber.dir/src/log.cpp.o.provides.build: CMakeFiles/liux_fiber.dir/src/log.cpp.o


CMakeFiles/liux_fiber.dir/src/util.cpp.o: CMakeFiles/liux_fiber.dir/flags.make
CMakeFiles/liux_fiber.dir/src/util.cpp.o: ../src/util.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/worksapce/sylarServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/liux_fiber.dir/src/util.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/liux_fiber.dir/src/util.cpp.o -c /root/worksapce/sylarServer/src/util.cpp

CMakeFiles/liux_fiber.dir/src/util.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/liux_fiber.dir/src/util.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /root/worksapce/sylarServer/src/util.cpp > CMakeFiles/liux_fiber.dir/src/util.cpp.i

CMakeFiles/liux_fiber.dir/src/util.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/liux_fiber.dir/src/util.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /root/worksapce/sylarServer/src/util.cpp -o CMakeFiles/liux_fiber.dir/src/util.cpp.s

CMakeFiles/liux_fiber.dir/src/util.cpp.o.requires:

.PHONY : CMakeFiles/liux_fiber.dir/src/util.cpp.o.requires

CMakeFiles/liux_fiber.dir/src/util.cpp.o.provides: CMakeFiles/liux_fiber.dir/src/util.cpp.o.requires
	$(MAKE) -f CMakeFiles/liux_fiber.dir/build.make CMakeFiles/liux_fiber.dir/src/util.cpp.o.provides.build
.PHONY : CMakeFiles/liux_fiber.dir/src/util.cpp.o.provides

CMakeFiles/liux_fiber.dir/src/util.cpp.o.provides.build: CMakeFiles/liux_fiber.dir/src/util.cpp.o


# Object files for target liux_fiber
liux_fiber_OBJECTS = \
"CMakeFiles/liux_fiber.dir/src/fiber.cpp.o" \
"CMakeFiles/liux_fiber.dir/src/thread.cpp.o" \
"CMakeFiles/liux_fiber.dir/src/config.cpp.o" \
"CMakeFiles/liux_fiber.dir/src/log.cpp.o" \
"CMakeFiles/liux_fiber.dir/src/util.cpp.o"

# External object files for target liux_fiber
liux_fiber_EXTERNAL_OBJECTS =

../lib/libliux_fiber.so: CMakeFiles/liux_fiber.dir/src/fiber.cpp.o
../lib/libliux_fiber.so: CMakeFiles/liux_fiber.dir/src/thread.cpp.o
../lib/libliux_fiber.so: CMakeFiles/liux_fiber.dir/src/config.cpp.o
../lib/libliux_fiber.so: CMakeFiles/liux_fiber.dir/src/log.cpp.o
../lib/libliux_fiber.so: CMakeFiles/liux_fiber.dir/src/util.cpp.o
../lib/libliux_fiber.so: CMakeFiles/liux_fiber.dir/build.make
../lib/libliux_fiber.so: CMakeFiles/liux_fiber.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/worksapce/sylarServer/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX shared library ../lib/libliux_fiber.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/liux_fiber.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/liux_fiber.dir/build: ../lib/libliux_fiber.so

.PHONY : CMakeFiles/liux_fiber.dir/build

CMakeFiles/liux_fiber.dir/requires: CMakeFiles/liux_fiber.dir/src/fiber.cpp.o.requires
CMakeFiles/liux_fiber.dir/requires: CMakeFiles/liux_fiber.dir/src/thread.cpp.o.requires
CMakeFiles/liux_fiber.dir/requires: CMakeFiles/liux_fiber.dir/src/config.cpp.o.requires
CMakeFiles/liux_fiber.dir/requires: CMakeFiles/liux_fiber.dir/src/log.cpp.o.requires
CMakeFiles/liux_fiber.dir/requires: CMakeFiles/liux_fiber.dir/src/util.cpp.o.requires

.PHONY : CMakeFiles/liux_fiber.dir/requires

CMakeFiles/liux_fiber.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/liux_fiber.dir/cmake_clean.cmake
.PHONY : CMakeFiles/liux_fiber.dir/clean

CMakeFiles/liux_fiber.dir/depend:
	cd /root/worksapce/sylarServer/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/worksapce/sylarServer /root/worksapce/sylarServer /root/worksapce/sylarServer/build /root/worksapce/sylarServer/build /root/worksapce/sylarServer/build/CMakeFiles/liux_fiber.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/liux_fiber.dir/depend

