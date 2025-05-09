#ifndef QUICKSORT_H
#define QUICKSORT_H

#include <cstddef>
#include <cstdint>
#include <vector>

class QuickSort {
 public:
  // External QuickSort with specified memory limit and arity
  void sort(std::vector<int64_t>& arr, size_t M, size_t a);

  // Auto-tuned external QuickSort (calculates optimal arity)
  void autoSort(std::vector<int64_t>& arr, size_t M);

 private:
  // Partition the array according to pivots
  void partition(std::vector<int64_t>& arr, std::vector<int64_t>& pivots,
                 std::vector<std::vector<int64_t>>& subarrays);

  // External QuickSort recursive implementation
  void externalQuickSort(std::vector<int64_t>& arr, size_t M, size_t a);
};

#endif  // QUICKSORT_H