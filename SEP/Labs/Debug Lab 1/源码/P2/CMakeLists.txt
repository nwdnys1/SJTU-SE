cmake_minimum_required(VERSION 3.20)
project(p2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_BUILD_TYPE "Debug")

add_executable(p2 main.cpp p2.cpp board.h canvas.h disk.h node.h queue.h rod.h stack.h)
