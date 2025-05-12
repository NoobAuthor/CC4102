#ifndef TEST_GENERATOR_H
#define TEST_GENERATOR_H

#include <cstddef>
#include <cstdint>
#include <vector>

/**
 * @brief Generates a dataset of integers based on the specified parameters.
 * @param size The size of the dataset to generate.
 */
std::vector<int> generateRandomData(std::size_t size);

/**
 * @brief Generates a dataset of 64-bit integers based on the specified
 * parameters.
 * @param size The size of the dataset to generate.
 */
std::vector<int64_t> generateRandomInt64Data(std::size_t size);

/**
 * @brief Generates sorted data of integers based on the specified parameters.
 * @param size The size of the dataset to generate.
 */
std::vector<int> generateSortedData(std::size_t size);

/**
 * @brief Generates reverse sorted data of integers based on the specified
 * parameters.
 * @param size The size of the dataset to generate.
 */
std::vector<int> generateReverseSortedData(std::size_t size);

/**
 * @brief Generates partially sorted data of integers based on the specified
 * parameters.
 * @param size The size of the dataset to generate.
 * @param sortedFraction The fraction of the dataset that should be sorted
 * (only applicable for "partially_sorted").
 */
std::vector<int> generatePartiallySortedData(std::size_t size,
                                             double sortedFraction);

#endif