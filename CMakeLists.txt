cmake_minimum_required(VERSION 3.7)
project(untitled)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cpp src/DepthBook.h src/FIXDecoder.h)
add_executable(untitled ${SOURCE_FILES})

#string(REPLACE "-O2" "-Os" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})

