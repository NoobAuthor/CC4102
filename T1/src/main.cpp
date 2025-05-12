#include <cstdint>
#include <iostream>
#include <vector>

#include "algorithms/mergesort.h"
#include "algorithms/quicksort.h"

int main() {
  // Example usage
  std::vector<int64_t> arr = {9, 3, 7, 5, 1, 8, 2, 6, 4};

  // Parameters for external sorting
  size_t M = 4;  // Maximum size that can be sorted in memory
  size_t a = 3;  // Number of subarrays for quicksort

  // Create sorter instances
  QuickSort quickSorter;

  // Make a copy for each sorter
  std::vector<int64_t> arr_quick = arr;

  // Sort using quicksort
  quickSorter.sort(arr_quick, M, a);

  // Print sorted array
  std::cout << "QuickSort result: ";
  for (const auto& num : arr_quick) {
    std::cout << num << " ";
  }
  std::cout << std::endl;

  return 0;
}