#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "algorithms/mergesort.h"
#include "algorithms/quicksort.h"
#include "utils/experiment_parameters.h"
#include "utils/file_handler.h"
#include "utils/sort_verification.h"
#include "utils/test_generator.h"
#include "utils/timer.h"

void runComparisonExperiment(const ExperimentParameters& params) {
  std::cout << "Running comparison experiment with " << params.dataType
            << " data of size " << params.dataSize
            << " (M=" << params.memoryLimit << ", "
            << (params.useAutoTuning ? "auto-tuned" : "manual")
            << " parameters)" << std::endl;

  std::vector<int> data;
  std::string filename = params.getDataFileName();

  try {
    if (params.dataType == "random") {
      data = generateRandomData(params.dataSize);
    } else if (params.dataType == "sorted") {
      data = generateSortedData(params.dataSize);
    } else if (params.dataType == "reverse") {
      data = generateReverseSortedData(params.dataSize);
    } else if (params.dataType == "partial") {
      data = generatePartiallySortedData(params.dataSize,
                                         params.partialSortFraction);
    } else {
      std::cerr << "Unknown data type: " << params.dataType << std::endl;
      return;
    }

    writeDataToFile(filename, data);
  } catch (const std::exception& e) {
    std::cerr << "Error generating/saving data: " << e.what() << std::endl;
    return;
  }

  std::vector<int64_t> mergeData(data.begin(), data.end());
  MergeSort mergeSorter;
  Timer mergeTimer;

  resetDiskCounters();

  mergeTimer.start();
  if (params.useAutoTuning) {
    mergeSorter.autoExternalSort(mergeData, params.memoryLimit);
  } else {
    mergeSorter.externalSort(mergeData, params.memoryLimit, params.arity);
  }
  mergeTimer.stop();

  size_t mergeDiskReads = getDiskReadCount();
  size_t mergeDiskWrites = getDiskWriteCount();

  bool mergeSorted = isSorted<int64_t>(mergeData);

  std::vector<int64_t> quickData(data.begin(), data.end());
  QuickSort quickSorter;
  Timer quickTimer;

  resetDiskCounters();

  quickTimer.start();
  if (params.useAutoTuning) {
    quickSorter.autoSort(quickData, params.memoryLimit);
  } else {
    quickSorter.sort(quickData, params.memoryLimit, params.arity);
  }
  quickTimer.stop();

  size_t quickDiskReads = getDiskReadCount();
  size_t quickDiskWrites = getDiskWriteCount();

  bool quickSorted = isSorted<int64_t>(quickData);

  std::cout << "Results for " << params.dataType << " data of size "
            << params.dataSize << ":" << std::endl;
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

  std::string tuningType = params.useAutoTuning ? "auto" : "manual";
  std::ofstream resultFile("data/comparison_results_" + tuningType + ".csv",
                           std::ios::app);
  if (resultFile) {
    resultFile << params.dataType << "," << params.dataSize << ","
               << params.memoryLimit << "," << mergeTimer.getDuration() << ","
               << mergeDiskReads << "," << mergeDiskWrites << ","
               << quickTimer.getDuration() << "," << quickDiskReads << ","
               << quickDiskWrites << ","
               << (mergeTimer.getDuration() / quickTimer.getDuration())
               << std::endl;
  }
}

int main(int argc, char* argv[]) {
  bool runLarge = (argc > 1 && std::string(argv[1]) == "large");
  bool useAutoTuning = true;

  if (argc > 2) {
    std::string tuningParam = argv[2];
    if (tuningParam == "manual") useAutoTuning = false;
  }

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
        if (M > size / 10) continue;

        ExperimentParameters params;
        params.dataSize = size;
        params.dataType = type;
        params.memoryLimit = M;
        params.useAutoTuning = useAutoTuning;

        runComparisonExperiment(params);
      }
    }
  }

  return 0;
}