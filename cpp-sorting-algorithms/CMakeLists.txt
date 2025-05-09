cmake_minimum_required(VERSION 3.10)
project(external_sorting_algorithms VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type configuration
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -Wextra -pedantic")

# Add the sort_parameters.cpp file to the list of sources
file(GLOB_RECURSE SOURCES
    "src/algorithms/*.cpp"
    "src/utils/*.cpp"  # This should include sort_parameters.cpp
)

# Include directories
include_directories(include)

# Find required packages
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)
find_package(Threads REQUIRED)

# Create directory structure
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/sequences)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/data/results)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/results)

# Sorting library
set(SORTING_LIB_SOURCES
    src/algorithms/mergesort.cpp
    src/algorithms/quicksort.cpp
    src/utils/file_handler.cpp
    src/utils/timer.cpp
    src/utils/test_generator.cpp
    src/utils/sort_parameters.cpp
)

add_library(sorting_lib STATIC ${SORTING_LIB_SOURCES})
target_link_libraries(sorting_lib PRIVATE Threads::Threads)

# Data generation executable
add_executable(data_generator tests/data_generator.cpp)
target_link_libraries(data_generator sorting_lib)
set_target_properties(data_generator PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Optimal arity finder
add_executable(optimal_arity_finder tests/optimal_arity_finder.cpp)
target_link_libraries(optimal_arity_finder sorting_lib)
set_target_properties(optimal_arity_finder PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Experiment runner
add_executable(experiment_runner tests/experiment_runner.cpp)
target_link_libraries(experiment_runner sorting_lib)
set_target_properties(experiment_runner PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Unit test executables
add_executable(test_mergesort tests/test_mergesort.cpp)
target_link_libraries(test_mergesort sorting_lib)

add_executable(test_quicksort tests/test_quicksort.cpp)
target_link_libraries(test_quicksort sorting_lib)

# Custom targets for experiment workflow
add_custom_target(generate_data
    COMMAND ${CMAKE_BINARY_DIR}/bin/data_generator
    DEPENDS data_generator
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating all required data sequences"
)

add_custom_target(find_optimal_arity
    COMMAND ${CMAKE_BINARY_DIR}/bin/optimal_arity_finder
    DEPENDS optimal_arity_finder generate_data
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Finding optimal arity using binary search"
)

# Targets for running experiments
add_custom_target(run_experiments_all
    COMMAND ${CMAKE_BINARY_DIR}/bin/experiment_runner
    DEPENDS experiment_runner find_optimal_arity
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running experiments on all data sizes"
)

# Targets for specific data sizes
foreach(SIZE RANGE 4 60 4)
    add_custom_target(run_experiments_${SIZE}M
        COMMAND ${CMAKE_BINARY_DIR}/bin/experiment_runner ${SIZE}
        DEPENDS experiment_runner find_optimal_arity
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running experiments on ${SIZE}M data size"
    )
endforeach()

# Visualization targets
add_custom_target(visualize_results
    COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/visualize_results.py
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating visualizations from experiment results"
)

# Master target to run entire experiment process
add_custom_target(run_complete_experiment
    DEPENDS 
        generate_data
        find_optimal_arity
        run_experiments_all
        visualize_results
    COMMENT "Running complete experiment workflow"
)

# Target to run tests
add_custom_target(run_tests
    DEPENDS
        test_mergesort
        test_quicksort
    COMMAND ${CMAKE_BINARY_DIR}/test_mergesort
    COMMAND ${CMAKE_BINARY_DIR}/test_quicksort
    COMMENT "Running unit tests for sorting algorithms"
)

# Clean targets
add_custom_target(clean_data
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/data/sequences
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/data/sequences
    COMMENT "Cleaning generated data sequences"
)

add_custom_target(clean_results
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/data/results
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/results
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/data/results
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/results
    COMMENT "Cleaning experiment results"
)

# Documentation about available targets
message(STATUS "Available targets:")
message(STATUS "  make                  - Build all executables")
message(STATUS "  make run_tests        - Run unit tests")
message(STATUS "  make generate_data    - Generate all required data sequences")
message(STATUS "  make find_optimal_arity - Find optimal arity value")
message(STATUS "  make run_experiments_all - Run all experiments")
message(STATUS "  make run_experiments_NM - Run experiments for specific size (replace N with 4, 8, etc.)")
message(STATUS "  make visualize_results - Create visualization plots")
message(STATUS "  make run_complete_experiment - Run entire workflow")
message(STATUS "  make clean_data       - Clean generated data")
message(STATUS "  make clean_results    - Clean experiment results")