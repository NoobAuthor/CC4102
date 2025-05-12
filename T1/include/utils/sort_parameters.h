#ifndef SORT_PARAMETERS_H
#define SORT_PARAMETERS_H

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

// Define constants according to requirements
constexpr size_t MB = 1024 * 1024;   // 1 megabyte in bytes
constexpr size_t M_SIZE = 50 * MB;   // 50 MB as specified in requirements
constexpr size_t BLOCK_SIZE = 4096;  // Standard disk block size in bytes
constexpr size_t INT64_SIZE =
    sizeof(int64_t);  // Size of 64-bit integer (8 bytes)
constexpr size_t INTS_PER_BLOCK =
    BLOCK_SIZE / INT64_SIZE;  // Number of integers per block

/**
 * Convert MB to element count (considering 64-bit integers)
 *
 * @param mbSize Size in megabytes
 * @return Number of 64-bit integers that can fit in the specified MB
 */
inline size_t mbToElementCount(size_t mbSize) {
  return (mbSize * MB) / INT64_SIZE;
}

/**
 * Calculate the optimal arity (fan-in/fan-out) for external sorting algorithms
 * based on available memory and buffer size.
 *
 * @param M Maximum memory size in bytes that can be used
 * @param bufferElements Number of elements that can fit in a buffer
 * @return The calculated optimal arity
 */
size_t calculateOptimalArity(size_t M, size_t bufferElements);

/**
 * Calculate the optimal buffer size for external sorting.
 *
 * @param M Maximum memory size in bytes
 * @param totalElements Total number of elements in the dataset
 * @param arity The merge arity being used
 * @return Optimal buffer size in number of elements
 */
size_t calculateOptimalBufferSize(size_t M, size_t totalElements, size_t arity);

/**
 * Find the optimal arity using binary search to determine the best performance.
 *
 * @param testData Vector of data to use for testing different arity values
 * @param testWithArity Function that tests a given arity and returns execution
 * time
 * @return The arity value that results in the best performance
 */
size_t findOptimalArityBinarySearch(
    const std::vector<int64_t>& testData,
    std::function<double(const std::vector<int64_t>&, size_t, size_t)>
        testWithArity);

#endif  // SORT_PARAMETERS_H