cmake_minimum_required(VERSION 3.20)
project(Breakout VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

set(COMPILE_OPTS -Wall -Wextra -pedantic -ggdb)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(COMPILE_OPTS /Wall)
endif()

# Add debugging option
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_definitions(-DDEBUG)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
  # Remove all compile options in release mode
  set(COMPILE_OPTS)
endif()


# Set up dependencies
add_subdirectory(deps)
set(INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(src)
