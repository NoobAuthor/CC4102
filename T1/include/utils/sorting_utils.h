#ifndef SORTING_UTILS_H
#define SORTING_UTILS_H

#include <algorithm>
#include <vector>

// Simple function to check if a vector is sorted
template <typename T>
bool isSorted(const std::vector<T>& vec) {
  return std::is_sorted(vec.begin(), vec.end());
}

#endif  // SORTING_UTILS_H