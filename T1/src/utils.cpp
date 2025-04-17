#include "utils.hpp"

#include <random>

namespace sorting_project {

void merge(uint64_t* arr, size_t left, size_t mid, size_t right) {
  size_t n1 = mid - left + 1;
  size_t n2 = right - mid;
  std::vector<uint64_t> left_arr(n1), right_arr(n2);
  for (size_t i = 0; i < n1; ++i) left_arr[i] = arr[left + i];
  for (size_t i = 0; i < n2; ++i) right_arr[i] = arr[mid + 1 + i];
  size_t i = 0, j = 0, k = left;
  while (i < n1 && j < n2) {
    if (left_arr[i] <= right_arr[j])
      arr[k++] = left_arr[i++];
    else
      arr[k++] = right_arr[j++];
  }
  while (i < n1) arr[k++] = left_arr[i++];
  while (j < n2) arr[k++] = right_arr[j++];
}

void in_memory_mergesort(uint64_t* arr, size_t left, size_t right) {
  if (left < right) {
    size_t mid = left + (right - left) / 2;
    in_memory_mergesort(arr, left, mid);
    in_memory_mergesort(arr, mid + 1, right);
    merge(arr, left, mid, right);
  }
}

size_t binary_search(const uint64_t* arr, size_t size, uint64_t key) {
  size_t left = 0, right = size;
  while (left < right) {
    size_t mid = left + (right - left) / 2;
    if (arr[mid] <= key)
      left = mid + 1;
    else
      right = mid;
  }
  return left;
}

void custom_shuffle(uint64_t* arr, size_t size) {
  std::mt19937_64 gen(std::random_device{}());
  for (size_t i = size - 1; i > 0; --i) {
    size_t j = gen() % (i + 1);
    std::swap(arr[i], arr[j]);
  }
}

}  // namespace sorting_project