#ifndef MERGESORT_H
#define MERGESORT_H

#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief MergeSort class provides methods for sorting arrays using the merge
 * sort algorithm.
 */
class MergeSort {
 public:
  /**
   * @brief Sorts an array of integers using the merge sort algorithm.
   * @param arr The array to be sorted.
   */
  void sort(std::vector<int>& arr);

  /**
   * @brief Sorts an array of integers using the external merge sort algorithm.
   * @param arr The array to be sorted.
   * @param M The number of runs to be created.
   * @param a The size of each run.
   */
  void externalSort(std::vector<int64_t>& arr, size_t M, size_t a);

  /**
   * @brief Sorts an array of integers using the auto external merge sort
   * algorithm.
   * @param arr The array to be sorted.
   * @param M The number of runs to be created.
   */
  void autoExternalSort(std::vector<int64_t>& arr, size_t M);

 private:
  /**
   * @brief Merges two sorted subarrays into a single sorted array.
   * @param arr The array containing the subarrays.
   * @param left The starting index of the left subarray.
   * @param mid The ending index of the left subarray.
   * @param right The ending index of the right subarray.
   */
  void merge(std::vector<int>& arr, int left, int mid, int right);

  /**
   * @brief Merges two sorted subarrays into a single sorted array.
   * @param arr The array containing the subarrays.
   * @param left The starting index of the left subarray.
   * @param mid The ending index of the left subarray.
   * @param right The ending index of the right subarray.
   */
  void mergeSort(std::vector<int>& arr, int left, int right);

  /**
   * @brief Creates initial runs of sorted data from the input array.
   * @param arr The input array.
   * @param runSize The size of each run.
   * @param tempDir The directory to store temporary files.
   */
  std::vector<std::string> createInitialRuns(const std::vector<int64_t>& arr,
                                             size_t runSize,
                                             const std::string& tempDir);

  /**
   * @brief Merges multiple sorted runs into a single sorted array.
   * @param runFiles The list of sorted run files.
   * @param outputFile The output file to store the merged result.
   * @param M The number of runs to be merged.
   * @param a The size of each run.
   */
  void mergeRuns(const std::vector<std::string>& runFiles,
                 const std::string& outputFile, size_t M, size_t a);

  /**
   * @brief Merges multiple sorted runs into a single sorted array.
   * @param runFiles The list of sorted run files.
   * @param output The output array to store the merged result.
   * @param M The number of runs to be merged.
   */
  void mergeSortedRuns(const std::vector<std::string>& runFiles,
                       std::vector<int64_t>& output, size_t M, size_t a);
};

#endif