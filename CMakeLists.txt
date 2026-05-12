cmake_minimum_required(VERSION 3.15)
project(jsonny LANGUAGES CXX)

add_library(jsonny STATIC 
    src/document.cpp 
)

target_include_directories(jsonny PUBLIC include)
target_compile_features(jsonny PUBLIC cxx_std_17)