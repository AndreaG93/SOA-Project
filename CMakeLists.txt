cmake_minimum_required(VERSION 3.10)
project(SOA-Project)

LINK_LIBRARIES("pthread")
set(CMAKE_C_STANDARD 11)

execute_process(
        COMMAND uname -r
        OUTPUT_VARIABLE KERNEL_RELEASE
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Find the headers
find_path(KERNELHEADERS_DIR
        include/linux/user.h
        PATHS /usr/src/linux-headers-${KERNEL_RELEASE}
        )

message(STATUS "Kernel release: ${KERNEL_RELEASE}")
message(STATUS "Kernel headers: ${KERNELHEADERS_DIR}")

set(KERNELHEADERS_INCLUDE_DIRS
        ${KERNELHEADERS_DIR}/include
        ${KERNELHEADERS_DIR}/arch/x86/include
        CACHE PATH "Kernel headers include dirs"
        )


add_executable(SOA-Project WaitFreeQueue/WaitQueue.c WaitFreeQueue/WaitQueue.h SynchronizationPrimitives/SynchronizationPrimitives.c SynchronizationPrimitives/SynchronizationPrimitives.h Module/TimeMessageSystemModule.c Module/TimeMessageSystemModule.h)