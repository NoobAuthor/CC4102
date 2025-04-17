#pragma once
#include <vector>

namespace sorting_project {
// Performs in-memory Mergesort on arr[left..right].
void in_memory_mergesort(std::vector<int>& arr, int left, int right);

// Performs binary search to find the partition for key.
void binary_search(std::vector<int>& arr, int left, int right, int key);

// Shuffles arr[0..size-1] randomly.
void custom_shuffle(std::vector<int>& arr, int size);
}  // namespace sorting_project