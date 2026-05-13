cmake_minimum_required(VERSION 3.15)
project(jsonny LANGUAGES CXX)

add_library(jsonny STATIC 
    src/jsonny.cpp
    src/lexer.cpp
)

target_include_directories(jsonny PUBLIC include)
target_compile_features(jsonny PUBLIC cxx_std_17)

option(JSONNY_BUILD_TESTS "Build unit tests" OFF)
if(JSONNY_BUILD_TESTS)
    add_subdirectory(tests)
endif()