cmake_minimum_required(VERSION 3.16)
project(DepGraph LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(depgraph_core
    src/graph.cpp
    src/tarjan.cpp
    src/condensation.cpp
    src/topological_sort.cpp
    src/output.cpp)
target_include_directories(depgraph_core PUBLIC src)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(depgraph_core PRIVATE -Wall -Wextra -Wpedantic)
endif()
add_executable(depgraph src/main.cpp)
target_link_libraries(depgraph PRIVATE depgraph_core)

enable_testing()
add_executable(scc_detection tests/scc_detection.cpp)
target_link_libraries(scc_detection PRIVATE depgraph_core)
add_test(NAME scc_detection COMMAND scc_detection)

add_executable(dependency_order tests/dependency_order.cpp)
target_link_libraries(dependency_order PRIVATE depgraph_core)
add_test(NAME dependency_order COMMAND dependency_order)

add_test(NAME console_examples COMMAND ${CMAKE_COMMAND}
    -DPROGRAM=$<TARGET_FILE:depgraph>
    -DEXAMPLES=${CMAKE_CURRENT_SOURCE_DIR}/examples
    -DTEST_DIR=${CMAKE_CURRENT_BINARY_DIR}
    -P ${CMAKE_CURRENT_SOURCE_DIR}/tests/check_cli.cmake)
