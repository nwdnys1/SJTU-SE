cmake_minimum_required(VERSION 3.20)
project(p1)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE "Debug")

add_executable(p1 main.cpp p1.cpp solution.h)