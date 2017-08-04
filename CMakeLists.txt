cmake_minimum_required(VERSION 3.7)
project(depthbook-constructor)

set(CMAKE_CXX_STANDARD 11)

# Compiler settings

SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")

# Build

set(SOURCE_FILES src/main.cpp src/DepthBook.h src/DepthBook.cpp src/FIXDecoder.h src/FIXDecoder.cpp src/FileProcessor.cpp src/FileProcessor.h)
add_executable(depthbook-constructor ${SOURCE_FILES})

# Link

target_link_libraries(depthbook-constructor)

#string(REPLACE "-O2" "-Os" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})

