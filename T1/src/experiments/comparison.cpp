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
  std::vector<int> data = generateRandomData(10000);
  writeToFile(filename, data);

  std::vector<int> dataForMergeSort = readFromFile(filename);
  std::vector<int> dataForQuickSort = dataForMergeSort;

  Timer mergeSortTimer;
  MergeSort mergeSort;
  mergeSortTimer.start();
  mergeSort.sort(dataForMergeSort);
  mergeSortTimer.stop();
  double mergeSortDuration = mergeSortTimer.getDuration();

  Timer quickSortTimer;
  QuickSort quickSort;
  quickSortTimer.start();
  std::vector<int64_t> longData(dataForQuickSort.begin(),
                                dataForQuickSort.end());
  size_t M = 1000;
  size_t a = 4;
  quickSort.sort(longData, M, a);
  quickSortTimer.stop();
  double quickSortDuration = quickSortTimer.getDuration();

  std::cout << "MergeSort execution time: " << mergeSortDuration << " seconds"
            << std::endl;
  std::cout << "QuickSort execution time: " << quickSortDuration << " seconds"
            << std::endl;
}

int main() {
  std::string filename = "data/test_data.bin";
  compareSortingAlgorithms(filename);
  return 0;
}