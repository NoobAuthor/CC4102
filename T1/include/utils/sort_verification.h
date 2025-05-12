#ifndef SORT_VERIFICATION_H
#define SORT_VERIFICATION_H

#include <algorithm>
#include <string>
#include <vector>

/**
 * Verifies if a collection is sorted in ascending order
 *
 * @tparam T The data type of elements in the collection
 * @tparam Container The container type (defaults to std::vector)
 * @param collection The collection to verify
 * @return true if the collection is sorted, false otherwise
 */
template <typename T,
          template <typename, typename = std::allocator<T>> class Container =
              std::vector>
bool isSorted(const Container<T>& collection) {
  if (collection.empty() || collection.size() == 1) {
    return true;  // Empty or single-element collections are always sorted
  }

  for (size_t i = 1; i < collection.size(); i++) {
    if (collection[i] < collection[i - 1]) {
      return false;
    }
  }
  return true;
}

/**
 * Verifies if a collection is sorted according to a custom comparator
 *
 * @tparam T The data type of elements in the collection
 * @tparam Container The container type
 * @tparam Compare The comparator function type
 * @param collection The collection to verify
 * @param comp The comparator function
 * @return true if the collection is sorted according to the comparator, false
 * otherwise
 */
template <typename T,
          template <typename, typename = std::allocator<T>> class Container =
              std::vector,
          typename Compare>
bool isSorted(const Container<T>& collection, Compare comp) {
  if (collection.empty() || collection.size() == 1) {
    return true;
  }

  for (size_t i = 1; i < collection.size(); i++) {
    if (comp(collection[i], collection[i - 1])) {
      return false;
    }
  }
  return true;
}

/**
 * Reports detailed information about why a collection is not sorted
 *
 * @tparam T The data type of elements in the collection
 * @tparam Container The container type
 * @param collection The collection to verify
 * @return A string describing why the collection is not sorted, or empty string
 * if sorted
 */
template <typename T,
          template <typename, typename = std::allocator<T>> class Container =
              std::vector>
std::string getSortingViolation(const Container<T>& collection) {
  if (collection.empty() || collection.size() == 1) {
    return "";  // Empty or single-element collections are always sorted
  }

  for (size_t i = 1; i < collection.size(); i++) {
    if (collection[i] < collection[i - 1]) {
      return "Collection not sorted at index " + std::to_string(i) + ": " +
             std::to_string(collection[i - 1]) + " > " +
             std::to_string(collection[i]);
    }
  }
  return "";
}

#endif  // SORT_VERIFICATION_H