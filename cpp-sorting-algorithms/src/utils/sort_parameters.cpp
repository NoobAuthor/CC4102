#include "utils/sort_parameters.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <vector>

size_t calculateOptimalArity(size_t M, size_t bufferElements) {
  // For MergeSort:
  // The optimal arity is determined by how many buffers we can fit in memory
  // We need (arity + 1) buffers for the merge operation

  // Maximum number of buffers we can have (accounting for some overhead)
  const size_t maxBuffers = M / (bufferElements * sizeof(int64_t));

  // We need at least 3 buffers (arity of 2)
  if (maxBuffers < 3) {
    return 2;  // Minimum viable arity
  }

  // Optimal arity is maxBuffers - 1 (one buffer is for output)
  size_t arity = maxBuffers - 1;

  // Cap at a reasonable maximum (to prevent excessive file handle usage)
  const size_t MAX_REASONABLE_ARITY = 16;
  arity = std::min(arity, MAX_REASONABLE_ARITY);

  return arity;
}

size_t calculateOptimalBufferSize(size_t M, size_t totalElements,
                                  size_t arity) {
  // Element size (assuming 64-bit integers)
  const size_t elementSize = sizeof(int64_t);

  // Calculate how many elements we can fit in total
  // Leave some memory for overhead (use 90% of available memory)
  const size_t totalBufferElements = (M * 0.9) / elementSize;

  // Divide by number of buffers needed
  size_t elementsPerBuffer = totalBufferElements / (arity + 1);

  // Ensure at least 1 element per buffer
  if (elementsPerBuffer < 1) {
    elementsPerBuffer = 1;
  }

  // Ensure the buffer isn't too large compared to total elements
  // Each buffer should ideally be at most totalElements / (arity * 2)
  size_t maxRecommendedSize = totalElements / (arity * 2);
  if (maxRecommendedSize < 1) {
    maxRecommendedSize = 1;
  }

  return std::min(elementsPerBuffer, maxRecommendedSize);
}

size_t findOptimalArityBinarySearch(
    const std::vector<int64_t>& testData,
    std::function<double(const std::vector<int64_t>&, size_t, size_t)>
        testWithArity) {
  // Binary search in range [2, INTS_PER_BLOCK]
  size_t minArity = 2;
  size_t maxArity = INTS_PER_BLOCK;

  std::cout << "Starting binary search for optimal arity in range [" << minArity
            << ", " << maxArity << "]" << std::endl;

  // If range is small, test all values
  if (maxArity - minArity <= 10) {
    std::vector<std::pair<size_t, double>> results;
    for (size_t a = minArity; a <= maxArity; a++) {
      double time = testWithArity(testData, M_SIZE, a);
      results.push_back({a, time});
      std::cout << "Tested arity " << a << ": " << time << " seconds"
                << std::endl;
    }

    // Find arity with minimum time
    auto minResult = std::min_element(
        results.begin(), results.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });

    std::cout << "Optimal arity found: " << minResult->first << " with time "
              << minResult->second << " seconds" << std::endl;
    return minResult->first;
  }

  // Binary search for optimal arity
  while (maxArity - minArity > 1) {
    size_t mid = minArity + (maxArity - minArity) / 2;

    // Test lower half boundary
    double timeLower = testWithArity(testData, M_SIZE, mid);
    std::cout << "Tested arity " << mid << ": " << timeLower << " seconds"
              << std::endl;

    // Test slightly above to determine direction
    size_t midUp = std::min(mid + 1, maxArity);
    double timeUpper = testWithArity(testData, M_SIZE, midUp);
    std::cout << "Tested arity " << midUp << ": " << timeUpper << " seconds"
              << std::endl;

    if (timeLower <= timeUpper) {
      // Optimal is in the lower range or at mid
      maxArity = mid;
    } else {
      // Optimal is in the upper range
      minArity = mid + 1;
    }

    // If we're down to a small range, do linear search for more accuracy
    if (maxArity - minArity <= 4) {
      std::vector<std::pair<size_t, double>> results;
      for (size_t a = minArity; a <= maxArity; a++) {
        double time = testWithArity(testData, M_SIZE, a);
        results.push_back({a, time});
        std::cout << "Tested arity " << a << ": " << time << " seconds"
                  << std::endl;
      }

      // Find arity with minimum time
      auto minResult = std::min_element(
          results.begin(), results.end(),
          [](const auto& a, const auto& b) { return a.second < b.second; });

      std::cout << "Optimal arity found: " << minResult->first << " with time "
                << minResult->second << " seconds" << std::endl;
      return minResult->first;
    }
  }

  // Final comparison of minArity and maxArity
  double timeMin = testWithArity(testData, M_SIZE, minArity);
  double timeMax = testWithArity(testData, M_SIZE, maxArity);

  std::cout << "Final comparison:" << std::endl;
  std::cout << "Arity " << minArity << ": " << timeMin << " seconds"
            << std::endl;
  std::cout << "Arity " << maxArity << ": " << timeMax << " seconds"
            << std::endl;

  size_t result = (timeMin <= timeMax) ? minArity : maxArity;
  std::cout << "Optimal arity found: " << result << std::endl;

  return result;
}