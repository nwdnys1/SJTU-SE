cmake_minimum_required(VERSION 3.10)

project(Tree)

add_executable(Tree main.cpp tree.h)
target_compile_features(Tree PRIVATE cxx_std_17)
