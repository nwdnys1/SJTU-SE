cmake_minimum_required(VERSION 3.20)
project(p4)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE "Debug")

add_executable(p4 main.cpp p4.cpp solution.h)