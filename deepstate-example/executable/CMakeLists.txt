cmake_minimum_required(VERSION 2.8)

project(Executable C CXX)

add_executable(Distributive Distributive.cpp)

find_library(DEEPSTATE_LIB deepstate deepstate32 ${CMAKE_SOURCE_DIR}/deepstate/src/include)
target_link_libraries(Distributive deepstate)

# ${CMAKE_SOURCE_DIR}/deepstate/src/include)
