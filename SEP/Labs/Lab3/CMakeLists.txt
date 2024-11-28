cmake_minimum_required(VERSION 3.20)
project(lab3)

set(CMAKE_CXX_STANDARD 11)

add_executable(lab3 hanoi.cpp board.cpp board.h canvas.h queue.h stack.h)
