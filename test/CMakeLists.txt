cmake_minimum_required(VERSION 3.7)
project(tests)

set(CMAKE_CXX_STANDARD 11)

# Add Google Test

add_subdirectory(../lib/googletest)
include_directories(../lib/googletest/googletest/include)
include_directories(../lib/googletest/googlemock/include)

# Compiler settings

SET(CMAKE_CXX_FLAGS_RELEASE "-O2")
SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")

# Build

set(TEST_SOURCE_FILES main.cpp ../src/DepthBook.h ../src/DepthBook.cpp ../src/FIXDecoder.h ../src/FIXDecoder.cpp DepthBookTest.cpp DepthListTest.cpp)
add_executable(tests ${TEST_SOURCE_FILES})
# Link

target_link_libraries(tests gtest gtest_main)

#string(REPLACE "-O2" "-Os" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})

