set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILES
    Timer.h
    DepthBook.h
    DepthBook.cpp
    FIXDecoder.h
    FIXDecoder.cpp
    FileProcessor.cpp
    FileProcessor.h
    FastDecoder.cpp
    FastDecoder.h Timer.h)

add_library(OrderBookAggregator ${SOURCE_FILES})
add_executable(depthbook-constructor main.cpp)
target_link_libraries(depthbook-constructor OrderBookAggregator)
