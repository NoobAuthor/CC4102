#ifndef MERGESORT_H
#define MERGESORT_H

#include <cstdint>
#include <string>
#include <vector>

class MergeSort {
 public:
  // Standard in-memory sort
  void sort(std::vector<int>& arr);

  // External memory sort with memory constraint M and specified arity
  void externalSort(std::vector<int64_t>& arr, size_t M, size_t a);

  // Auto-tuned external sort (calculates optimal arity)
  void autoExternalSort(std::vector<int64_t>& arr, size_t M);

 private:
  // Standard merge function for in-memory sorting
  void merge(std::vector<int>& arr, int left, int mid, int right);

  // Standard recursive mergesort for in-memory sorting
  void mergeSort(std::vector<int>& arr, int left, int right);

  // Create initial sorted runs for external sorting
  std::vector<std::string> createInitialRuns(const std::vector<int64_t>& arr,
                                             size_t runSize,
                                             const std::string& tempDir);

  // Merge K sorted runs using K-way merge
  void mergeRuns(const std::vector<std::string>& runFiles,
                 const std::string& outputFile, size_t M, size_t a);

  // Build a single merged output from multiple runs
  void mergeSortedRuns(const std::vector<std::string>& runFiles,
                       std::vector<int64_t>& output, size_t M, size_t a);
};

#endif  // MERGESORT_H