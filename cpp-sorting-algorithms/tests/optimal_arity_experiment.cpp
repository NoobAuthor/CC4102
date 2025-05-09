#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "algorithms/mergesort.h"
#include "algorithms/quicksort.h"
#include "utils/file_handler.h"
#include "utils/sort_parameters.h"
#include "utils/test_generator.h"
#include "utils/timer.h"

/**
 * Experiment to test the effect of different arity values on sorting
 * performance and compare with the theoretically optimal arity.
 */
void runOptimalArityExperiment(std::size_t size, const std::string& dataType,
                               std::size_t M) {
  std::cout << "Running optimal arity experiment with " << dataType
            << " data of size " << size << " (M=" << M << ")" << std::endl;

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

  // Calculate theoretical optimal arity
  size_t elementSize = sizeof(int64_t);
  size_t blockSize = 4096;  // Typical filesystem block size
  size_t bufferElements = blockSize / elementSize;
  size_t optimalArity = calculateOptimalArity(M, bufferElements);

  std::cout << "Theoretical optimal arity for M=" << M << ": " << optimalArity
            << std::endl;

  // Define a range of arities to test
  // Test around the optimal value (half to double)
  size_t minArity = std::max(size_t(2), optimalArity / 2);
  size_t maxArity = std::min(size_t(16), optimalArity * 2);

  std::vector<size_t> arities;
  for (size_t a = minArity; a <= maxArity; a++) {
    arities.push_back(a);
  }

  // Ensure we test the calculated optimal arity
  if (std::find(arities.begin(), arities.end(), optimalArity) ==
      arities.end()) {
    arities.push_back(optimalArity);
    std::sort(arities.begin(), arities.end());
  }

  // Open results file
  std::ofstream resultFile("data/optimal_arity_results.csv", std::ios::app);
  if (resultFile.is_open()) {
    resultFile << "Algorithm,DataType,Size,MemorySize,Arity,OptimalArity,Time,"
                  "DiskReads,DiskWrites\n";
  } else {
    std::cerr << "Could not open results file" << std::endl;
    return;
  }

  // Test MergeSort with different arities
  for (size_t a : arities) {
    // Convert to int64_t for external sort
    std::vector<int64_t> testData(data.begin(), data.end());

    // Reset disk counters
    resetDiskCounters();

    // Run MergeSort and measure performance
    MergeSort sorter;
    Timer timer;

    std::cout << "Testing MergeSort with arity " << a << "..." << std::endl;

    timer.start();
    sorter.externalSort(testData, M, a);
    timer.stop();

    // Check if data is correctly sorted
    bool isSorted = true;
    for (std::size_t i = 1; i < testData.size(); i++) {
      if (testData[i] < testData[i - 1]) {
        isSorted = false;
        break;
      }
    }

    if (!isSorted) {
      std::cerr << "Warning: MergeSort result is not sorted!" << std::endl;
    }

    // Output results
    std::cout << "MergeSort (a=" << a << "): Time=" << std::fixed
              << std::setprecision(6) << timer.getDuration()
              << "s, Reads=" << getDiskReadCount()
              << ", Writes=" << getDiskWriteCount() << std::endl;

    // Save results
    resultFile << "MergeSort," << dataType << "," << size << "," << M << ","
               << a << "," << optimalArity << "," << timer.getDuration() << ","
               << getDiskReadCount() << "," << getDiskWriteCount() << std::endl;
  }

  // Test QuickSort with different arities
  for (size_t a : arities) {
    // Convert to int64_t for external sort
    std::vector<int64_t> testData(data.begin(), data.end());

    // Reset disk counters
    resetDiskCounters();

    // Run QuickSort and measure performance
    QuickSort sorter;
    Timer timer;

    std::cout << "Testing QuickSort with arity " << a << "..." << std::endl;

    timer.start();
    sorter.sort(testData, M, a);
    timer.stop();

    // Check if data is correctly sorted
    bool isSorted = true;
    for (std::size_t i = 1; i < testData.size(); i++) {
      if (testData[i] < testData[i - 1]) {
        isSorted = false;
        break;
      }
    }

    if (!isSorted) {
      std::cerr << "Warning: QuickSort result is not sorted!" << std::endl;
    }

    // Output results
    std::cout << "QuickSort (a=" << a << "): Time=" << std::fixed
              << std::setprecision(6) << timer.getDuration()
              << "s, Reads=" << getDiskReadCount()
              << ", Writes=" << getDiskWriteCount() << std::endl;

    // Save results
    resultFile << "QuickSort," << dataType << "," << size << "," << M << ","
               << a << "," << optimalArity << "," << timer.getDuration() << ","
               << getDiskReadCount() << "," << getDiskWriteCount() << std::endl;
  }

  // Finally, test the auto-tuned versions
  {
    // Test auto-tuned MergeSort
    std::vector<int64_t> testData(data.begin(), data.end());
    resetDiskCounters();

    MergeSort sorter;
    Timer timer;

    std::cout << "Testing auto-tuned MergeSort..." << std::endl;

    timer.start();
    sorter.autoExternalSort(testData, M);
    timer.stop();

    // Check if data is correctly sorted
    bool isSorted = true;
    for (std::size_t i = 1; i < testData.size(); i++) {
      if (testData[i] < testData[i - 1]) {
        isSorted = false;
        break;
      }
    }

    if (!isSorted) {
      std::cerr << "Warning: Auto-tuned MergeSort result is not sorted!"
                << std::endl;
    }

    std::cout << "Auto-tuned MergeSort: Time=" << std::fixed
              << std::setprecision(6) << timer.getDuration()
              << "s, Reads=" << getDiskReadCount()
              << ", Writes=" << getDiskWriteCount() << std::endl;

    resultFile << "MergeSort-Auto," << dataType << "," << size << "," << M
               << "," << optimalArity << "," << optimalArity << ","
               << timer.getDuration() << "," << getDiskReadCount() << ","
               << getDiskWriteCount() << std::endl;
  }

  {
    // Test auto-tuned QuickSort
    std::vector<int64_t> testData(data.begin(), data.end());
    resetDiskCounters();

    QuickSort sorter;
    Timer timer;

    std::cout << "Testing auto-tuned QuickSort..." << std::endl;

    timer.start();
    sorter.autoSort(testData, M);
    timer.stop();

    // Check if data is correctly sorted
    bool isSorted = true;
    for (std::size_t i = 1; i < testData.size(); i++) {
      if (testData[i] < testData[i - 1]) {
        isSorted = false;
        break;
      }
    }

    if (!isSorted) {
      std::cerr << "Warning: Auto-tuned QuickSort result is not sorted!"
                << std::endl;
    }

    std::cout << "Auto-tuned QuickSort: Time=" << std::fixed
              << std::setprecision(6) << timer.getDuration()
              << "s, Reads=" << getDiskReadCount()
              << ", Writes=" << getDiskWriteCount() << std::endl;

    resultFile << "QuickSort-Auto," << dataType << "," << size << "," << M
               << "," << optimalArity << "," << optimalArity << ","
               << timer.getDuration() << "," << getDiskReadCount() << ","
               << getDiskWriteCount() << std::endl;
  }

  resultFile.close();
}

int main(int argc, char* argv[]) {
  // Create header in results file if it doesn't exist
  std::ofstream checkFile("data/optimal_arity_results.csv");
  if (checkFile.is_open()) {
    checkFile << "Algorithm,DataType,Size,MemorySize,Arity,OptimalArity,Time,"
                 "DiskReads,DiskWrites\n";
    checkFile.close();
  }

  // Run experiments with different parameters
  std::vector<std::size_t> sizes = {
      100000};  // Focus on medium size for testing arity
  if (argc > 1 && std::string(argv[1]) == "large") {
    sizes.push_back(1000000);
  }

  // Test with random data first, can expand to other types if needed
  std::vector<std::string> dataTypes = {"random"};

  // Test with different memory sizes
  std::vector<std::size_t> memorySizes = {1000, 10000};

  for (const auto& type : dataTypes) {
    for (const auto& size : sizes) {
      for (const auto& M : memorySizes) {
        runOptimalArityExperiment(size, type, M);
      }
    }
  }

  return 0;
}