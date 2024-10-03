cmake_minimum_required(VERSION 3.10)
project(CJIT)

set (CMAKE_C_STANDARD 17)
set (CMAKE_CXX_STANDARD 17)

find_package(Clang REQUIRED)

include_directories(${CLANG_INCLUDE_DIRS})

add_definitions(${CLANG_DEFINITIONS})

add_executable(cjit cjit.cpp)

target_link_libraries(cjit clangCodeGen)
