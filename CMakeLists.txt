cmake_minimum_required(VERSION 3.7)

project(depthbook-constructor)

set(CMAKE_CXX_STANDARD 11)

# Compiler settings

SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")

add_subdirectory(src)

enable_testing()
add_subdirectory(test)

