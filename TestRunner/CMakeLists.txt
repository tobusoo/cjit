cmake_minimum_required(VERSION 3.10)

set (CMAKE_C_STANDARD 17)
set (CMAKE_CXX_STANDARD 17)

find_package(LLVM REQUIRED CONFIG)

add_executable(test-runner
    main.cpp
    TestRunner.cpp
    Benchmarks/BenchRunner.cpp
    Benchmarks/Sink/SinkBenchmark.cpp
)

target_include_directories(test-runner
    PRIVATE ${LLVM_INCLUDE_DIRS}
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../Optimizer
)

target_compile_definitions(test-runner PRIVATE ${LLVM_DEFINITIONS})

target_link_libraries(test-runner 
    PRIVATE LLVM
    PUBLIC optimizer
)
