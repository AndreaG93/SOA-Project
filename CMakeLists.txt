cmake_minimum_required(VERSION 3.10)
project(SOA-Project)

LINK_LIBRARIES("pthread")
set(CMAKE_C_STANDARD 11)

add_executable(SOA-Project WaitFreeQueue/WaitFreeQueue.c WaitFreeQueue/WaitFreeQueue.h Hardware/SynchronizationPrimitives.h Hardware/MemoryAlignment.h Hardware/MemoryAlignment.c WaitFreeQueue/DataCell.h main.c WaitFreeQueue/Requests.c WaitFreeQueue/Requests.h)
