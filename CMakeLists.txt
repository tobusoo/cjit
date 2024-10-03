cmake_minimum_required(VERSION 3.10)
project(CJIT)

set (CMAKE_C_STANDARD 17)
set (CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED)

add_executable(cjit cjit.cpp)

target_include_directories(cjit PRIVATE ${LLVM_INCLUDE_DIRS})
target_include_directories(cjit PRIVATE ${CLANG_INCLUDE_DIRS})

target_compile_definitions(cjit PRIVATE ${LLVM_DEFINITIONS})
target_compile_definitions(cjit PRIVATE ${CLANG_DEFINITIONS})

target_link_libraries(cjit LLVM clang-cpp)
