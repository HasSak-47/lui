
cmake_minimum_required(VERSION 3.20)
project(lui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project structure
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# Find Lua
find_package(PkgConfig REQUIRED)
pkg_check_modules(LUA REQUIRED lua5.4)

# Source files
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${SRC_DIR}/*.cpp")

# Library target
add_library(tui STATIC ${SOURCES})
target_include_directories(lui PUBLIC ${INCLUDE_DIR} ${LUA_INCLUDE_DIRS})
target_link_libraries(lui PUBLIC ${LUA_LIBRARIES})

# Executable (test)
add_executable(test ${SRC_DIR}/main.cpp)
target_link_libraries(test PRIVATE lui)

# Output paths
set_target_properties(lui PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
