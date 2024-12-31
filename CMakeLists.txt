cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)
PROJECT(InstRenderer)


include(CMake/third.cmake)
include(CMake/OutputSettings.cmake)
include(CMake/CompileShader.cmake)
include(CMake/CmakeMacros.cmake)

add_subdirectory(Source)