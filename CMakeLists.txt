cmake_minimum_required(VERSION 3.0)

project(depthbook-constructor)

set(CMAKE_CXX_STANDARD 11)

# Compiler settings

SET(CMAKE_CXX_FLAGS_RELEASE "-O3")
SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")

add_subdirectory(src)

enable_testing()
add_subdirectory(test)

