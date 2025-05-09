#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "algorithms/mergesort.h"
#include "utils/file_handler.h"
#include "utils/sort_parameters.h"
#include "utils/timer.h"

// Test function for MergeSort with a given arity
double testMergeSortArity(const std::vector<int64_t>& data, size_t M,
                          size_t a) {
  // Make a copy of the data
  std::vector<int64_t> testData = data;

  // Reset disk counters
  resetDiskCounters();

  // Create sorter and timer
  MergeSort sorter;
  Timer timer;

  // Run with specific arity and measure time
  timer.start();
  sorter.externalSort(testData, M, a);
  timer.stop();

  // Verify the results are sorted
  bool sorted = true;
  for (size_t i = 1; i < testData.size(); i++) {
    if (testData[i] < testData[i - 1]) {
      sorted = false;
      std::cerr << "Error: Result is not sorted for arity " << a << std::endl;
      break;
    }
  }

  if (!sorted) {
    // Return a very high time to indicate failure
    return std::numeric_limits<double>::max();
  }

  // Return the execution time
  return timer.getDuration();
}

int main() {
  // Load the 60M test data for optimal arity determination
  std::string testFile = "data/sequences/arity_test_60M.bin";
  std::cout << "Loading test data from " << testFile << std::endl;

  std::vector<int64_t> testData;
  try {
    testData = readInt64DataFromFile(testFile);
  } catch (const std::exception& e) {
    std::cerr << "Error loading test data: " << e.what() << std::endl;
    std::cerr << "Please run data_generator first to create the test data."
              << std::endl;
    return 1;
  }

  std::cout << "Loaded " << testData.size() << " elements." << std::endl;
  std::cout << "Starting binary search for optimal arity..." << std::endl;

  // Find optimal arity using binary search
  size_t optimalArity =
      findOptimalArityBinarySearch(testData, testMergeSortArity);

  // Save the result
  std::ofstream arityFile("data/optimal_arity.txt");
  arityFile << optimalArity << std::endl;
  arityFile.close();

  std::cout << "Optimal arity determination complete." << std::endl;
  std::cout << "Optimal arity: " << optimalArity << std::endl;
  std::cout << "This value will be used for both MergeSort and QuickSort."
            << std::endl;

  return 0;
}