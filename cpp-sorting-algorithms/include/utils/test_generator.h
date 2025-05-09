#ifndef TEST_GENERATOR_H
#define TEST_GENERATOR_H

#include <cstddef>  // For std::size_t
#include <cstdint>  // For int64_t
#include <vector>

// Use std::size_t instead of just size_t
std::vector<int> generateRandomData(std::size_t size);

// Update all other function declarations similarly
std::vector<int64_t> generateRandomInt64Data(std::size_t size);

std::vector<int> generateSortedData(std::size_t size);

std::vector<int> generateReverseSortedData(std::size_t size);

std::vector<int> generatePartiallySortedData(std::size_t size,
                                             double sortedFraction);

std::vector<int> generateTestData(std::size_t size);

#endif  // TEST_GENERATOR_H