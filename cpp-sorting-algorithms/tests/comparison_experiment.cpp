#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "algorithms/mergesort.h"
#include "algorithms/quicksort.h"
#include "utils/file_handler.h"
#include "utils/test_generator.h"
#include "utils/timer.h"

void runComparisonExperiment(std::size_t size, const std::string& dataType,
                             std::size_t M, bool useAutoTuning) {
  std::cout << "Running comparison experiment with " << dataType
            << " data of size " << size << " (M=" << M << ", "
            << (useAutoTuning ? "auto-tuned" : "manual") << " parameters)"
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

  // Default arity value for manual tuning
  size_t a = 4;  // Default arity for non-auto-tuned version

  // Test MergeSort
  std::vector<int64_t> mergeData(data.begin(), data.end());
  MergeSort mergeSorter;
  Timer mergeTimer;

  // Reset disk counters
  resetDiskCounters();

  mergeTimer.start();
  if (useAutoTuning) {
    mergeSorter.autoExternalSort(mergeData, M);
  } else {
    mergeSorter.externalSort(mergeData, M, a);
  }
  mergeTimer.stop();

  // Store MergeSort disk access counts
  size_t mergeDiskReads = getDiskReadCount();
  size_t mergeDiskWrites = getDiskWriteCount();

  bool mergeSorted = true;
  for (std::size_t i = 1; i < mergeData.size(); i++) {
    if (mergeData[i] < mergeData[i - 1]) {
      mergeSorted = false;
      break;
    }
  }

  // Test QuickSort
  std::vector<int64_t> quickData(data.begin(), data.end());
  QuickSort quickSorter;
  Timer quickTimer;

  // Reset disk counters
  resetDiskCounters();

  quickTimer.start();
  if (useAutoTuning) {
    quickSorter.autoSort(quickData, M);
  } else {
    quickSorter.sort(quickData, M, a);
  }
  quickTimer.stop();

  // Store QuickSort disk access counts
  size_t quickDiskReads = getDiskReadCount();
  size_t quickDiskWrites = getDiskWriteCount();

  bool quickSorted = true;
  for (std::size_t i = 1; i < quickData.size(); i++) {
    if (quickData[i] < quickData[i - 1]) {
      quickSorted = false;
      break;
    }
  }

  // Output results
  std::cout << "Results for " << dataType << " data of size " << size << ":"
            << std::endl;
  std::cout << "  MergeSort time: " << std::fixed << std::setprecision(6)
            << mergeTimer.getDuration()
            << " seconds (sorted: " << (mergeSorted ? "Yes" : "No") << ")"
            << std::endl;
  std::cout << "  MergeSort disk reads: " << mergeDiskReads << std::endl;
  std::cout << "  MergeSort disk writes: " << mergeDiskWrites << std::endl;
  std::cout << "  QuickSort time: " << std::fixed << std::setprecision(6)
            << quickTimer.getDuration()
            << " seconds (sorted: " << (quickSorted ? "Yes" : "No") << ")"
            << std::endl;
  std::cout << "  QuickSort disk reads: " << quickDiskReads << std::endl;
  std::cout << "  QuickSort disk writes: " << quickDiskWrites << std::endl;
  std::cout << "  Speedup: "
            << (mergeTimer.getDuration() / quickTimer.getDuration()) << "x"
            << std::endl;
  std::cout << std::endl;

  // Save results to file
  std::string tuningType = useAutoTuning ? "auto" : "manual";
  std::ofstream resultFile("data/comparison_results_" + tuningType + ".csv",
                           std::ios::app);
  if (resultFile) {
    resultFile << dataType << "," << size << "," << M << ","
               << mergeTimer.getDuration() << "," << mergeDiskReads << ","
               << mergeDiskWrites << "," << quickTimer.getDuration() << ","
               << quickDiskReads << "," << quickDiskWrites << ","
               << (mergeTimer.getDuration() / quickTimer.getDuration())
               << std::endl;
  }
}

int main(int argc, char* argv[]) {
  bool runLarge = (argc > 1 && std::string(argv[1]) == "large");
  bool useAutoTuning = true;  // Default to auto-tuning

  if (argc > 2) {
    std::string tuningParam = argv[2];
    if (tuningParam == "manual") useAutoTuning = false;
  }

  // Create header in results file if it doesn't exist
  std::string filename = "data/comparison_results_" +
                         std::string(useAutoTuning ? "auto" : "manual") +
                         ".csv";
  std::ifstream checkFile(filename);
  if (!checkFile.good()) {
    std::ofstream resultFile(filename);
    resultFile
        << "DataType,Size,MemorySize,MergeSort_Time,MergeSort_DiskReads,"
           "MergeSort_DiskWrites,"
        << "QuickSort_Time,QuickSort_DiskReads,QuickSort_DiskWrites,Speedup"
        << std::endl;
  }

  // Run experiments with different parameters
  std::vector<std::size_t> sizes = {1000, 10000, 100000};
  if (runLarge) {
    sizes.push_back(1000000);
  }

  std::vector<std::string> dataTypes = {"random", "sorted", "reverse",
                                        "partial"};
  std::vector<std::size_t> memorySizes = {1000, 10000};

  for (const auto& type : dataTypes) {
    for (const auto& size : sizes) {
      for (const auto& M : memorySizes) {
        if (M > size / 10)
          continue;  // Skip if memory is too large relative to data
        runComparisonExperiment(size, type, M, useAutoTuning);
      }
    }
  }

  return 0;
}