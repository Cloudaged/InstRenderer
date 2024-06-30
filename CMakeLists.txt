cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 17)
PROJECT(InstRenderer)

include(CMake/third.cmake)
include(CMake/OutputSettings.cmake)
add_subdirectory(Source)