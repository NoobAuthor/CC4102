#include <cassert>
#include <iostream>
#include <vector>

#include "algorithms/mergesort.h"
#include "utils/timer.h"

void testMergeSort() {
  MergeSort sorter;

  std::vector<int> arr1 = {};
  sorter.sort(arr1);
  assert(arr1.empty());

  std::vector<int> arr2 = {5};
  sorter.sort(arr2);
  assert(arr2 == std::vector<int>({5}));

  std::vector<int> arr3 = {1, 2, 3, 4, 5};
  sorter.sort(arr3);
  assert(arr3 == std::vector<int>({1, 2, 3, 4, 5}));

  std::vector<int> arr4 = {5, 4, 3, 2, 1};
  sorter.sort(arr4);
  assert(arr4 == std::vector<int>({1, 2, 3, 4, 5}));

  std::vector<int> arr5 = {3, 1, 4, 1, 5, 9, 2, 6, 5};
  sorter.sort(arr5);
  assert(arr5 == std::vector<int>({1, 1, 2, 3, 4, 5, 5, 6, 9}));

  std::cout << "All MergeSort tests passed!" << std::endl;
}

int main() {
  Timer timer;
  timer.start();
  testMergeSort();
  timer.stop();
  std::cout << "MergeSort tests executed in: " << timer.elapsed() << " seconds."
            << std::endl;
  return 0;
}