cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(cxx11_multi_thread)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

enable_testing()
add_subdirectory("chapter03 Sharing data between threads")
