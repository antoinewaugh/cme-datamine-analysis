
set(SOURCE_FILES
        DepthBook.h
        DepthBook.cpp
        FIXDecoder.h
        FIXDecoder.cpp
        FileProcessor.cpp
        FileProcessor.h
        FastDecoder.cpp
        FastDecoder.h)

add_library(OrderBookAggregator ${SOURCE_FILES})
add_executable(depthbook-constructor main.cpp)
target_link_libraries(depthbook-constructor OrderBookAggregator)
