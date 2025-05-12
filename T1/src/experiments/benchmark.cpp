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

  MergeSort mergeSort;
  Timer timer;
  timer.start();
  mergeSort.sort(data);
  timer.stop();
  std::cout << "MergeSort execution time: " << timer.getDuration() << " seconds"
            << std::endl;

  data = readDataFromFile(filename);
  QuickSort quickSort;
  timer.reset();
  timer.start();

  std::vector<int64_t> longData(data.begin(), data.end());
  size_t M = 1000;
  size_t a = 4;
  quickSort.sort(longData, M, a);

  timer.stop();
  std::cout << "QuickSort execution time: " << timer.getDuration() << " seconds"
            << std::endl;
}

int main() {
  std::string filename = "data/test_data.txt";
  benchmarkSortingAlgorithms(filename);
  return 0;
}