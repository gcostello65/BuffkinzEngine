# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

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

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/gregcostello/Documents/dev/c++/BuffkinzEngine

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/gregcostello/Documents/dev/c++/BuffkinzEngine/cmake-build-debug

# Utility rule file for shaders.

# Include any custom commands dependencies for this target.
include CMakeFiles/shaders.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/shaders.dir/progress.make

CMakeFiles/shaders: shaders/simple_shader.frag.spv
CMakeFiles/shaders: shaders/simple_shader.vert.spv

shaders/simple_shader.frag.spv: /Users/gregcostello/Documents/dev/c++/BuffkinzEngine/shaders/simple_shader.frag
shaders/simple_shader.frag.spv: shaders
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/gregcostello/Documents/dev/c++/BuffkinzEngine/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Compiling simple_shader.frag"
	/usr/local/bin/glslc -o /Users/gregcostello/Documents/dev/c++/BuffkinzEngine/cmake-build-debug/shaders/simple_shader.frag.spv /Users/gregcostello/Documents/dev/c++/BuffkinzEngine/shaders/simple_shader.frag

shaders/simple_shader.vert.spv: /Users/gregcostello/Documents/dev/c++/BuffkinzEngine/shaders/simple_shader.vert
shaders/simple_shader.vert.spv: shaders
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/gregcostello/Documents/dev/c++/BuffkinzEngine/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Compiling simple_shader.vert"
	/usr/local/bin/glslc -o /Users/gregcostello/Documents/dev/c++/BuffkinzEngine/cmake-build-debug/shaders/simple_shader.vert.spv /Users/gregcostello/Documents/dev/c++/BuffkinzEngine/shaders/simple_shader.vert

shaders:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/gregcostello/Documents/dev/c++/BuffkinzEngine/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Creating /Users/gregcostello/Documents/dev/c++/BuffkinzEngine/cmake-build-debug/shaders"
	/Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E make_directory /Users/gregcostello/Documents/dev/c++/BuffkinzEngine/cmake-build-debug/shaders

shaders: CMakeFiles/shaders
shaders: shaders
shaders: shaders/simple_shader.frag.spv
shaders: shaders/simple_shader.vert.spv
shaders: CMakeFiles/shaders.dir/build.make
.PHONY : shaders

# Rule to build all files generated by this target.
CMakeFiles/shaders.dir/build: shaders
.PHONY : CMakeFiles/shaders.dir/build

CMakeFiles/shaders.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/shaders.dir/cmake_clean.cmake
.PHONY : CMakeFiles/shaders.dir/clean

CMakeFiles/shaders.dir/depend:
	cd /Users/gregcostello/Documents/dev/c++/BuffkinzEngine/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/gregcostello/Documents/dev/c++/BuffkinzEngine /Users/gregcostello/Documents/dev/c++/BuffkinzEngine /Users/gregcostello/Documents/dev/c++/BuffkinzEngine/cmake-build-debug /Users/gregcostello/Documents/dev/c++/BuffkinzEngine/cmake-build-debug /Users/gregcostello/Documents/dev/c++/BuffkinzEngine/cmake-build-debug/CMakeFiles/shaders.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/shaders.dir/depend
