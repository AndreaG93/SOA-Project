cmake_minimum_required(VERSION 3.10)
project(SOA-Project)

LINK_LIBRARIES("pthread")
set(CMAKE_C_STANDARD 11)

add_executable(SOA-Project main.c WaitFreeQueue/LockFreeQueue.c WaitFreeQueue/LockFreeQueue.h WaitFreeQueue/ThreadsPool.c WaitFreeQueue/ThreadsPool.h)
