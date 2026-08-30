cmake_minimum_required(VERSION 3.14)
project(Koral VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(Koral INTERFACE)

target_include_directories(Koral INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
