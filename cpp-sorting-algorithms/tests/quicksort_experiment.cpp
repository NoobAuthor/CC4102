#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "algorithms/quicksort.h"
#include "utils/file_handler.h"
#include "utils/test_generator.h"
#include "utils/timer.h"

void runQuickSortExperiment(std::size_t size, const std::string& dataType,
                            std::size_t M, std::size_t a) {
  std::cout << "Running QuickSort experiment with " << dataType
            << " data of size " << size << " (M=" << M << ", a=" << a << ")"
            << std::endl;

  std::vector<int> data;
  std::string filename =
      "data/" + dataType + "_" + std::to_string(size) + ".bin";

  // Generate or load test data
  try {
    if (dataType == "random") {
      data = generateRandomData(size);
    } else if (dataType == "sorted") {
      data = generateSortedData(size);
    } else if (dataType == "reverse") {
      data = generateReverseSortedData(size);
    } else if (dataType == "partial") {
      data = generatePartiallySortedData(size, 0.7);  // 70% sorted
    } else {
      std::cerr << "Unknown data type: " << dataType << std::endl;
      return;
    }

    // Save data for potential reuse
    writeDataToFile(data, filename);
  } catch (const std::exception& e) {
    std::cerr << "Error generating/saving data: " << e.what() << std::endl;
    return;
  }

  // Convert to int64_t for QuickSort
  std::vector<int64_t> longData(data.begin(), data.end());

  // Reset disk counters
  resetDiskCounters();

  // Run QuickSort and measure performance
  QuickSort sorter;
  Timer timer;

  timer.start();
  sorter.sort(longData, M, a);
  timer.stop();

  // Verify the data is sorted
  bool isSorted = true;
  for (std::size_t i = 1; i < longData.size(); i++) {
    if (longData[i] < longData[i - 1]) {
      isSorted = false;
      break;
    }
  }

  // Output results
  std::cout << "QuickSort (" << dataType << ", n=" << size << ", M=" << M
            << ", a=" << a << "):" << std::endl;
  std::cout << "  Time: " << std::fixed << std::setprecision(6)
            << timer.getDuration() << " seconds" << std::endl;
  std::cout << "  Disk reads: " << getDiskReadCount() << std::endl;
  std::cout << "  Disk writes: " << getDiskWriteCount() << std::endl;
  std::cout << "  Correctly sorted: " << (isSorted ? "Yes" : "No") << std::endl;
  std::cout << std::endl;

  // Save results to file
  std::ofstream resultFile("data/quicksort_results.csv", std::ios::app);
  if (resultFile) {
    resultFile << dataType << "," << size << "," << M << "," << a << ","
               << timer.getDuration() << "," << getDiskReadCount() << ","
               << getDiskWriteCount() << "," << (isSorted ? "1" : "0")
               << std::endl;
  }
}

int main(int argc, char* argv[]) {
  // Create header in results file if it doesn't exist
  std::ifstream checkFile("data/quicksort_results.csv");
  if (!checkFile.good()) {
    std::ofstream resultFile("data/quicksort_results.csv");
    resultFile
        << "DataType,Size,MemorySize,Subarrays,Time,DiskReads,DiskWrites,Sorted"
        << std::endl;
  }

  // Run experiments with different parameters
  std::vector<std::size_t> sizes = {1000, 10000, 100000};
  if (argc > 1 && std::string(argv[1]) == "large") {
    // Add large test size only when explicitly requested
    sizes.push_back(1000000);
  }

  std::vector<std::string> dataTypes = {"random", "sorted", "reverse",
                                        "partial"};

  // Parameters adjusted based on data size
  for (const auto& type : dataTypes) {
    for (const auto& size : sizes) {
      // For smaller arrays, test more combinations
      if (size <= 10000) {
        runQuickSortExperiment(size, type, 100, 2);
        runQuickSortExperiment(size, type, 100, 4);
        runQuickSortExperiment(size, type, 100, 8);
        runQuickSortExperiment(size, type, 1000, 2);
        runQuickSortExperiment(size, type, 1000, 4);
        runQuickSortExperiment(size, type, 1000, 8);
      }
      // For medium size arrays
      else if (size <= 100000) {
        runQuickSortExperiment(size, type, 100, 2);
        runQuickSortExperiment(size, type, 100, 4);
        runQuickSortExperiment(size, type, 100, 8);
        runQuickSortExperiment(size, type, 1000, 2);
        runQuickSortExperiment(size, type, 1000, 4);
        runQuickSortExperiment(size, type, 1000, 8);
        runQuickSortExperiment(size, type, 10000, 2);
        runQuickSortExperiment(size, type, 10000, 4);
        runQuickSortExperiment(size, type, 10000, 8);
      }
      // For large arrays, be more conservative with parameters
      else {
        runQuickSortExperiment(size, type, 1000, 2);
        runQuickSortExperiment(size, type, 1000, 4);
        runQuickSortExperiment(size, type, 10000, 2);
        runQuickSortExperiment(size, type, 10000, 4);
      }
    }
  }

  return 0;
}