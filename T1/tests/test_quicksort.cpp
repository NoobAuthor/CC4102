#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

#include "algorithms/quicksort.h"
#include "utils/test_generator.h"
#include "utils/timer.h"

void testQuickSort() {
  Timer timer;
  QuickSort qs;
  size_t M = 1000;
  size_t a = 4;

  std::vector<int> tempData1 = generateRandomData(1000);
  std::vector<int64_t> data1(tempData1.begin(), tempData1.end());
  std::vector<int64_t> sortedData1 = data1;

  timer.start();
  qs.sort(data1, M, a);
  timer.stop();
  std::sort(sortedData1.begin(), sortedData1.end());
  assert(data1 == sortedData1);
  std::cout << "Test case 1 executed in: " << timer.getElapsedTime()
            << " seconds.\n";

  std::vector<int> tempData2 = generateSortedData(1000);
  std::vector<int64_t> data2(tempData2.begin(), tempData2.end());
  std::vector<int64_t> sortedData2 = data2;

  timer.start();
  qs.sort(data2, M, a);
  timer.stop();
  assert(data2 == sortedData2);
  std::cout << "Test case 2 executed in: " << timer.getElapsedTime()
            << " seconds.\n";

  std::vector<int> tempData3 = generateReverseSortedData(1000);
  std::vector<int64_t> data3(tempData3.begin(), tempData3.end());
  std::vector<int64_t> sortedData3 = data3;

  timer.start();
  qs.sort(data3, M, a);
  timer.stop();
  std::sort(sortedData3.begin(), sortedData3.end());
  assert(data3 == sortedData3);
  std::cout << "Test case 3 executed in: " << timer.getElapsedTime()
            << " seconds.\n";
}

int main() {
  testQuickSort();
  std::cout << "All QuickSort tests passed!" << std::endl;
  return 0;
}