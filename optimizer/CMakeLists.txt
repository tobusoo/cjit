cmake_minimum_required(VERSION 3.10)
project(CJITOptimizer)

set (CMAKE_C_STANDARD 17)
set (CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG)

add_library(optimizer SHARED optimizer.cpp)

target_include_directories(optimizer PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_definitions(optimizer PRIVATE ${LLVM_DEFINITIONS})

target_link_libraries(optimizer LLVM)
