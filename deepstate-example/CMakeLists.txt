cmake_minimum_required(VERSION 2.8)

project(distributive)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_COLOR_MAKEFILE   ON)

SET(CMAKE_CXX_FLAGS "-std=c++11 -O3")

#include deepstate lib
add_subdirectory(deepstate)
include_directories(${PROJECT_SOURCE_DIR}/deepstate/src/include)
link_directories(${PROJECT_SOURCE_DIR}/deepstate/src/lib)
#link our executables and deepstate lib
add_subdirectory(executable)

