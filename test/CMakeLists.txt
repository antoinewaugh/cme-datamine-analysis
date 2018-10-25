cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add Google Test

add_subdirectory(../lib/googletest googletest-build)
include_directories(../lib/googletest/googletest/include)
include_directories(../lib/googletest/googlemock/include)

# Compiler settings

SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -g c++17")
#SET(CMAKE_CXX_FLAGS_DEBUG  "-O0 -g")

# Build

set(TEST_SOURCE_FILES  DepthBookTest.cpp DepthListTest.cpp main.cpp)
add_executable(tests ${TEST_SOURCE_FILES})
# Link

target_link_libraries(
        tests
        OrderBookAggregator
        gtest
        gtest_main)

#string(REPLACE "-O2" "-Os" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})

