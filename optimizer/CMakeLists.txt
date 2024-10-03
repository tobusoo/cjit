cmake_minimum_required(VERSION 3.10)

set (CMAKE_C_STANDARD 17)
set (CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_library(optimizer SHARED optimizer.cpp)

llvm_map_components_to_libnames(llvm_libs core )

target_link_libraries(optimizer ${llvm_libs})
