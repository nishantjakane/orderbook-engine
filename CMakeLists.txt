cmake_minimum_required(VERSION 3.20)

project(OrderBookCLI)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
    OrderBookTest

    tests/test.cpp

    src/MatchingEngine.cpp
    src/OrderBook.cpp
)

target_include_directories(
    OrderBookTest
    PRIVATE
    include
)