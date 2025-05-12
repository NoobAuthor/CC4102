#ifndef QUICKSORT_H
#define QUICKSORT_H

#include <cstddef>
#include <cstdint>
#include <vector>

/**
 * @brief QuickSort class provides methods for sorting arrays using the
 * quicksort algorithm.
 */
class QuickSort {
 public:
  /**
   * @brief Sorts an array of integers using the quicksort algorithm.
   * @param arr The array to be sorted.
   * @param M The number of runs to be created.
   * @param a The size of each run.
   */
  void sort(std::vector<int64_t>& arr, size_t M, size_t a);

  /**
   * @brief Auto-sorts an array of integers using the quicksort algorithm.
   * @param arr The array to be sorted.
   * @param M The number of runs to be created.
   */
  void autoSort(std::vector<int64_t>& arr, size_t M);

 private:
  /**
   * @brief Partitions the array into subarrays based on the pivot values.
   * @param arr The array to be partitioned.
   * @param pivots The pivot values.
   * @param subarrays The resulting subarrays.
   */
  void partition(std::vector<int64_t>& arr, std::vector<int64_t>& pivots,
                 std::vector<std::vector<int64_t>>& subarrays);

  /**
   * @brief External quicksort algorithm for sorting large arrays.
   * @param arr The array to be sorted.
   * @param M The number of runs to be created.
   * @param a The size of each run.
   */
  void externalQuickSort(std::vector<int64_t>& arr, size_t M, size_t a);
};

#endif