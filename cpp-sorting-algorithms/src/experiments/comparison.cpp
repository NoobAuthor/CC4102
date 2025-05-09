#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

#include "algorithms/mergesort.h"
#include "algorithms/quicksort.h"
#include "utils/file_handler.h"
#include "utils/test_generator.h"
#include "utils/timer.h"

void compareSortingAlgorithms(const std::string& filename) {
  // Generate test data and write to file
  std::vector<int> data = generateTestData(10000);  // Adjust size as needed
  writeToFile(filename, data);

  // Read data from file for sorting
  std::vector<int> dataForMergeSort = readFromFile(filename);
  std::vector<int> dataForQuickSort = dataForMergeSort;

  // Measure MergeSort execution time
  Timer mergeSortTimer;
  MergeSort mergeSort;
  mergeSortTimer.start();
  mergeSort.sort(dataForMergeSort);
  mergeSortTimer.stop();
  double mergeSortDuration = mergeSortTimer.getDuration();

  // Measure QuickSort execution time
  Timer quickSortTimer;
  QuickSort quickSort;
  quickSortTimer.start();
  // Convert to int64_t as required by QuickSort
  std::vector<int64_t> longData(dataForQuickSort.begin(),
                                dataForQuickSort.end());
  size_t M = 1000;  // Example value
  size_t a = 4;     // Example value
  quickSort.sort(longData, M, a);
  quickSortTimer.stop();
  double quickSortDuration = quickSortTimer.getDuration();

  // Output results
  std::cout << "MergeSort execution time: " << mergeSortDuration << " seconds"
            << std::endl;
  std::cout << "QuickSort execution time: " << quickSortDuration << " seconds"
            << std::endl;
}

int main() {
  std::string filename = "data/test_data.bin";  // Example file path
  compareSortingAlgorithms(filename);
  return 0;
}