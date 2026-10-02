cmake_minimum_required(VERSION 3.20)
project(algorithm-visualizer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Algorithm code: graph, Dijkstra, BFS, events. No networking dependencies.
# INTERFACE for now; becomes a STATIC library once sources are added.
add_library(algoviz_core INTERFACE)
target_include_directories(algoviz_core INTERFACE backend/include)
target_compile_features(algoviz_core INTERFACE cxx_std_20)

function(algoviz_warnings target)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Wconversion)
    elseif(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    endif()
endfunction()

# HTTP server executable (HTTP layer is added in a later phase).
add_executable(server backend/src/main.cpp)
target_link_libraries(server PRIVATE algoviz_core)
algoviz_warnings(server)

# Tests, run with: ctest --test-dir build
enable_testing()
add_executable(tests backend/tests/test_main.cpp)
target_link_libraries(tests PRIVATE algoviz_core)
algoviz_warnings(tests)
add_test(NAME tests COMMAND tests)

# Benchmarks
add_executable(benchmarks backend/benchmarks/bench_main.cpp)
target_link_libraries(benchmarks PRIVATE algoviz_core)
algoviz_warnings(benchmarks)
