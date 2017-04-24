cmake_minimum_required(VERSION 3.7)
project(cme-datamine-analysis)

set(CMAKE_CXX_STANDARD 11)

# Add Google Test

add_subdirectory(lib/googletest)
include_directories(lib/googletest/googletest/include)
include_directories(lib/googletest/googlemock/include)

# Compiler settings

SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")

# Build

set(SOURCE_FILES src/main.cpp src/DepthBook.h src/DepthBook.cpp src/FIXDecoder.h src/FIXDecoder.cpp test/DepthBookTest.cpp test/DepthListTest.cpp)
add_executable(cme-datamine-analysis ${SOURCE_FILES})

# Link

target_link_libraries(cme-datamine-analysis gtest gtest_main)

#string(REPLACE "-O2" "-Os" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})

