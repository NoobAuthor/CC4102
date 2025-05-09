# cpp-sorting-algorithms

This project implements two sorting algorithms: External Mergesort and Quicksort. It includes functionality to compare their execution times and disk access during sorting operations.

## Project Structure

- **include/**: Contains header files for algorithms and utility functions.
  - **algorithms/**: 
    - `mergesort.h`: Declaration of the `MergeSort` class.
    - `quicksort.h`: Declaration of the `QuickSort` class.
  - **utils/**: 
    - `file_handler.h`: Utility functions for file handling.
    - `timer.h`: Timer class for measuring execution time.
    - `test_generator.h`: Functions for generating test data.

- **src/**: Contains the implementation of algorithms and utilities.
  - **algorithms/**: 
    - `mergesort.cpp`: Implementation of the external mergesort algorithm.
    - `quicksort.cpp`: Implementation of the quicksort algorithm.
  - **utils/**: 
    - `file_handler.cpp`: Implementation of file handling functions.
    - `timer.cpp`: Implementation of the Timer class.
    - `test_generator.cpp`: Implementation of test data generation functions.
  - **experiments/**: 
    - `benchmark.cpp`: Code to benchmark sorting algorithms.
    - `comparison.cpp`: Code to compare the performance of mergesort and quicksort.
  - `main.cpp`: Entry point of the application.

- **tests/**: Contains unit tests for the sorting algorithms.
  - `test_mergesort.cpp`: Unit tests for the mergesort implementation.
  - `test_quicksort.cpp`: Unit tests for the quicksort implementation.

- **data/**: Directory for data files (currently empty).

- **CMakeLists.txt**: Configuration file for CMake.

- **.gitignore**: Specifies files and directories to be ignored by Git.

- **README.md**: Documentation for the project.

## Setup Instructions

1. Clone the repository:
   ```
   git clone <repository-url>
   cd cpp-sorting-algorithms
   ```

2. Create a build directory and navigate into it:
   ```
   mkdir build
   cd build
   ```

3. Run CMake to configure the project:
   ```
   cmake ..
   ```

4. Build the project:
   ```
   make
   ```

5. Run the application:
   ```
   ./cpp-sorting-algorithms
   ```

## Usage

The application will execute the sorting algorithms and provide a comparison of their performance in terms of execution time and disk access. You can modify the test data generation parameters in `test_generator.h` to test with different datasets.

## Contributing

Contributions are welcome! Please feel free to submit a pull request or open an issue for any suggestions or improvements.