cmake_minimum_required(VERSION 3.16)
project(riscv-emulator VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/cpu.cpp
    src/memory.cpp
    src/bus.cpp
    src/device.cpp
    src/uart.cpp
    src/clint.cpp
    src/plic.cpp
    src/elf_loader.cpp
    src/main.cpp
)

# Headers
set(HEADERS
    include/cpu.h
    include/memory.h
    include/bus.h
    include/device.h
    include/uart.h
    include/clint.h
    include/plic.h
    include/elf_loader.h
    include/constants.h
    include/types.h
)

# Create library
add_library(riscv-core ${SOURCES} ${HEADERS})

# Main executable
add_executable(riscv-emulator src/main.cpp)
target_link_libraries(riscv-emulator riscv-core)

# Tests
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# Examples
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# Installation
install(TARGETS riscv-emulator DESTINATION bin)
