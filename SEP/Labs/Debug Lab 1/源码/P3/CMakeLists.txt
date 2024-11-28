cmake_minimum_required(VERSION 3.20)
project(p3)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE "Debug")

add_executable(p3 main.cpp p3.cpp solution.h)