#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "algorithms/mergesort.h"
#include "algorithms/quicksort.h"
#include "utils/file_handler.h"
#include "utils/test_generator.h"
#include "utils/timer.h"

void benchmarkSortingAlgorithms(const std::string& filename) {
  std::vector<int> data = readDataFromFile(filename);

  // Benchmark MergeSort
  MergeSort mergeSort;
  Timer timer;
  timer.start();
  mergeSort.sort(data);
  timer.stop();
  std::cout << "MergeSort execution time: " << timer.getDuration() << " seconds"
            << std::endl;

  // Benchmark QuickSort
  data = readDataFromFile(filename);  // Reload data for fair comparison
  QuickSort quickSort;
  timer.reset();  // Reset timer before second test
  timer.start();

  // Convert to int64_t as required by QuickSort
  std::vector<int64_t> longData(data.begin(), data.end());
  size_t M = 1000;  // Example value
  size_t a = 4;     // Example value
  quickSort.sort(longData, M, a);

  timer.stop();
  std::cout << "QuickSort execution time: " << timer.getDuration() << " seconds"
            << std::endl;
}

int main() {
  std::string filename = "data/test_data.txt";  // Example file path
  benchmarkSortingAlgorithms(filename);
  return 0;
}