cmake_minimum_required(VERSION 3.10)
project(CJIT)

set (CMAKE_C_STANDARD 17)
set (CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED)

# Set the LLVM include directory
include_directories(${LLVM_INCLUDE_DIRS} ${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
add_definitions(${CLANG_DEFINITIONS})

add_executable(cjit cjit.cpp)

# WARNING: Do not link with LLVM libraries here! It will result
# in duplicates coming from clang-cpp.
target_link_libraries(cjit ${CMAKE_DL_LIBS} pthread clang-cpp)
