cmake_minimum_required(VERSION 3.10)
project(SOA-Project)

LINK_LIBRARIES("pthread")
set(CMAKE_C_STANDARD 11)

add_executable(SOA-Project main.c UserSide/FileOperationsTests.c UserSide/FileOperationsTests.h)
