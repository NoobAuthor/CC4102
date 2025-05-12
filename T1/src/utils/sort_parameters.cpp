#include "utils/sort_parameters.h"

#include <algorithm>
#include <cmath>
#include <functional>
#include <iostream>
#include <limits>
#include <vector>

size_t calculateOptimalArity(size_t M, size_t bufferElements) {
  const size_t maxBuffers = M / (bufferElements * sizeof(int64_t));

  if (maxBuffers < 3) {
    return 2;
  }

  size_t arity = maxBuffers - 1;

  const size_t MAX_REASONABLE_ARITY = 16;
  arity = std::min(arity, MAX_REASONABLE_ARITY);

  return arity;
}

size_t calculateOptimalBufferSize(size_t M, size_t totalElements,
                                  size_t arity) {
  const size_t elementSize = sizeof(int64_t);

  const size_t totalBufferElements = (M * 0.9) / elementSize;

  size_t elementsPerBuffer = totalBufferElements / (arity + 1);

  if (elementsPerBuffer < 1) {
    elementsPerBuffer = 1;
  }

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
  size_t minArity = 2;
  size_t maxArity = INTS_PER_BLOCK;

  std::cout << "Starting binary search for optimal arity in range [" << minArity
            << ", " << maxArity << "]" << std::endl;

  if (maxArity - minArity <= 10) {
    std::vector<std::pair<size_t, double>> results;
    for (size_t a = minArity; a <= maxArity; a++) {
      double time = testWithArity(testData, M_SIZE, a);
      results.push_back({a, time});
      std::cout << "Tested arity " << a << ": " << time << " seconds"
                << std::endl;
    }

    auto minResult = std::min_element(
        results.begin(), results.end(),
        [](const auto& a, const auto& b) { return a.second < b.second; });

    std::cout << "Optimal arity found: " << minResult->first << " with time "
              << minResult->second << " seconds" << std::endl;
    return minResult->first;
  }

  while (maxArity - minArity > 1) {
    size_t mid = minArity + (maxArity - minArity) / 2;

    double timeLower = testWithArity(testData, M_SIZE, mid);
    std::cout << "Tested arity " << mid << ": " << timeLower << " seconds"
              << std::endl;

    size_t midUp = std::min(mid + 1, maxArity);
    double timeUpper = testWithArity(testData, M_SIZE, midUp);
    std::cout << "Tested arity " << midUp << ": " << timeUpper << " seconds"
              << std::endl;

    if (timeLower <= timeUpper) {
      maxArity = mid;
    } else {
      minArity = mid + 1;
    }

    if (maxArity - minArity <= 4) {
      std::vector<std::pair<size_t, double>> results;
      for (size_t a = minArity; a <= maxArity; a++) {
        double time = testWithArity(testData, M_SIZE, a);
        results.push_back({a, time});
        std::cout << "Tested arity " << a << ": " << time << " seconds"
                  << std::endl;
      }

      auto minResult = std::min_element(
          results.begin(), results.end(),
          [](const auto& a, const auto& b) { return a.second < b.second; });

      std::cout << "Optimal arity found: " << minResult->first << " with time "
                << minResult->second << " seconds" << std::endl;
      return minResult->first;
    }
  }

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