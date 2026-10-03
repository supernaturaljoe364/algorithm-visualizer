cmake_minimum_required(VERSION 3.20)
project(algorithm-visualizer VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

function(algoviz_warnings target)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wshadow -Wconversion)
    elseif(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    endif()
endfunction()

# The algorithms: Graph is header-only; Dijkstra and BFS are the two source files.
add_library(algoviz_core STATIC backend/src/dijkstra.cpp backend/src/bfs.cpp)
target_include_directories(algoviz_core PUBLIC backend/include)
target_compile_features(algoviz_core PUBLIC cxx_std_20)
algoviz_warnings(algoviz_core)

# HTTP server executable. Crow and standalone Asio are downloaded at configure time into
# build/_deps (nothing is installed system-wide). Only the server depends on them.
include(FetchContent)
FetchContent_Declare(asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-30-2
    GIT_SHALLOW TRUE)
FetchContent_Declare(crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.2.0
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(asio)
set(ASIO_INCLUDE_DIR ${asio_SOURCE_DIR}/asio/include CACHE PATH "Asio headers" FORCE)  # used by Crow's Findasio
set(CROW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CROW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CROW_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(crow)
# Treat Crow's headers as system headers so our -Wconversion etc. only apply to our code.
set_target_properties(Crow PROPERTIES
    INTERFACE_SYSTEM_INCLUDE_DIRECTORIES "$<TARGET_PROPERTY:Crow,INTERFACE_INCLUDE_DIRECTORIES>")

add_executable(server backend/src/main.cpp)
target_link_libraries(server PRIVATE algoviz_core Crow::Crow)
# Crow serves static files from this directory, at "/<file>" (e.g. /style.css -> frontend/style.css).
target_compile_definitions(server PRIVATE
    CROW_STATIC_DIRECTORY="${CMAKE_SOURCE_DIR}/frontend/"
    CROW_STATIC_ENDPOINT="/<path>")
algoviz_warnings(server)

# Tests, run with: ctest --test-dir build
enable_testing()
add_executable(tests backend/tests/tests.cpp)
target_link_libraries(tests PRIVATE algoviz_core)
algoviz_warnings(tests)
add_test(NAME tests COMMAND tests)

# Benchmarks
add_executable(benchmarks backend/benchmarks/benchmarks.cpp)
target_link_libraries(benchmarks PRIVATE algoviz_core)
target_include_directories(benchmarks PRIVATE backend/tests)  # for random_graph.h
algoviz_warnings(benchmarks)
