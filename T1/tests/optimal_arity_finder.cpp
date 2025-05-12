#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include "algorithms/mergesort.h"
#include "utils/file_handler.h"
#include "utils/file_manager.h"
#include "utils/sort_parameters.h"
#include "utils/sort_verification.h"
#include "utils/timer.h"

double testMergeSortArity(const std::vector<int64_t>& data, size_t M,
                          size_t a) {
  std::cout << "Testing arity: " << a << std::endl;

  size_t elementSize = sizeof(int64_t);
  size_t totalBuffers = a + 1;
  size_t bufferSize = (M * 0.9) / (totalBuffers * elementSize);

  if (bufferSize < 1000) {
    std::cout << "  Buffer size too small (" << bufferSize
              << " elements), skipping" << std::endl;
    return std::numeric_limits<double>::max();
  }

  size_t testSize = std::min(data.size(), size_t(10000000));
  std::vector<int64_t> testData(data.begin(), data.begin() + testSize);

  std::cout << "  Running test with " << testSize << " elements" << std::endl;
  std::cout << "  Buffer size: " << bufferSize << " elements" << std::endl;

  resetDiskCounters();

  MergeSort sorter;
  Timer timer;

  timer.start();
  sorter.externalSort(testData, M, a);
  timer.stop();

  double duration = timer.getDuration();
  std::cout << "  Time: " << duration << " seconds" << std::endl;
  std::cout << "  Disk reads: " << getDiskReadCount()
            << ", writes: " << getDiskWriteCount() << std::endl;

  if (!isSorted<int64_t>(testData)) {
    std::cerr << "  Error: Result is not sorted for arity " << a << std::endl;
    return std::numeric_limits<double>::max();
  }

  return duration;
}

size_t smartBinarySearch(
    const std::vector<int64_t>& data,
    std::function<double(const std::vector<int64_t>&, size_t, size_t)>
        testWithArity) {
  size_t minArity = 2;
  size_t maxArity = 512;

  std::cout << "Starting smart binary search for optimal arity in range ["
            << minArity << ", " << maxArity << "]" << std::endl;

  const std::vector<size_t> landmarks = {2, 4, 8, 16, 32, 64, 128, 256, 512};
  std::vector<std::pair<size_t, double>> results;

  std::cout << "Phase 1: Testing landmark arity values" << std::endl;
  for (size_t a : landmarks) {
    double time = testWithArity(data, M_SIZE, a);
    if (time < std::numeric_limits<double>::max()) {
      results.push_back({a, time});
      std::cout << "  Arity " << a << ": " << time << " seconds" << std::endl;
    }
  }

  if (results.empty()) {
    std::cerr
        << "No valid arity values found in landmarks! Using default arity=8"
        << std::endl;
    return 8;
  }

  auto bestLandmark = std::min_element(
      results.begin(), results.end(),
      [](const auto& a, const auto& b) { return a.second < b.second; });

  std::cout << "Best landmark arity: " << bestLandmark->first << " with time "
            << bestLandmark->second << " seconds" << std::endl;

  size_t bestArity = bestLandmark->first;
  double bestTime = bestLandmark->second;

  size_t lowerBound = bestArity / 2;
  if (lowerBound < 2) lowerBound = 2;

  size_t upperBound = bestArity * 2;
  if (upperBound > 512) upperBound = 512;

  if (upperBound - lowerBound <= 10) {
    std::cout << "Phase 2: Linear search in range [" << lowerBound << ", "
              << upperBound << "]" << std::endl;
    for (size_t a = lowerBound; a <= upperBound; a++) {
      if (std::find_if(results.begin(), results.end(), [a](const auto& p) {
            return p.first == a;
          }) != results.end()) {
        continue;
      }

      double time = testWithArity(data, M_SIZE, a);
      if (time < bestTime) {
        bestTime = time;
        bestArity = a;
        std::cout << "  New best arity: " << bestArity << " with time "
                  << bestTime << " seconds" << std::endl;
      }
    }
  } else {
    std::cout << "Phase 2: Strategic refinement around arity " << bestArity
              << std::endl;
    std::vector<size_t> refinementPoints;

    if (bestArity > 2) refinementPoints.push_back(bestArity - bestArity / 4);
    refinementPoints.push_back(bestArity + bestArity / 4);

    for (size_t a : refinementPoints) {
      if (a < 2 || a > 512) continue;

      if (std::find_if(results.begin(), results.end(), [a](const auto& p) {
            return p.first == a;
          }) != results.end()) {
        continue;
      }

      double time = testWithArity(data, M_SIZE, a);
      if (time < bestTime) {
        bestTime = time;
        bestArity = a;
        std::cout << "  New best arity: " << bestArity << " with time "
                  << bestTime << " seconds" << std::endl;
      }
    }
  }

  std::cout << "Final optimal arity determined: " << bestArity << " with time "
            << bestTime << " seconds" << std::endl;
  return bestArity;
}

int main() {
  FileManager& fileManager = FileManager::getInstance();

  std::string testFile = fileManager.findLargestArityTestFile();

  if (testFile.empty()) {
    testFile = "data/sequences/arity_test_60M.bin";
    std::cerr << "No arity test file found, will attempt to use default: "
              << testFile << std::endl;
  }

  std::cout << "Loading test data from " << testFile << std::endl;

  std::vector<int64_t> testData;
  try {
    testData = readInt64DataFromFile(testFile);
  } catch (const std::exception& e) {
    std::cerr << "Error loading test data: " << e.what() << std::endl;
    std::cerr << "Please run data_generator first to create the test data."
              << std::endl;
    return 1;
  }

  std::cout << "Loaded " << testData.size() << " elements." << std::endl;

  size_t optimalArity = smartBinarySearch(testData, testMergeSortArity);

  std::ofstream arityFile("data/optimal_arity.txt");
  arityFile << optimalArity << std::endl;
  arityFile.close();

  std::cout << "Optimal arity determination complete." << std::endl;
  std::cout << "Optimal arity: " << optimalArity << std::endl;
  std::cout << "This value will be used for both MergeSort and QuickSort."
            << std::endl;

  return 0;
}