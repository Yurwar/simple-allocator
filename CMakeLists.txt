cmake_minimum_required(VERSION 3.14)
project(allocator C)

set(CMAKE_C_STANDARD 99)

add_executable(allocator main.c allocator.c include/allocator.h)