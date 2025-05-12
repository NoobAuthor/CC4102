#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "algorithms/mergesort.h"
#include "utils/file_handler.h"
#include "utils/sort_verification.h"
#include "utils/test_generator.h"
#include "utils/timer.h"

void runMergeSortExperiment(std::size_t size, const std::string& dataType,
                            std::size_t M) {
  std::cout << "Running MergeSort experiment with " << dataType
            << " data of size " << size << " (M=" << M << ")" << std::endl;

  std::vector<int> data;
  std::string filename =
      "data/" + dataType + "_" + std::to_string(size) + ".bin";

  try {
    if (dataType == "random") {
      data = generateRandomData(size);
    } else if (dataType == "sorted") {
      data = generateSortedData(size);
    } else if (dataType == "reverse") {
      data = generateReverseSortedData(size);
    } else if (dataType == "partial") {
      data = generatePartiallySortedData(size, 0.7);
    } else {
      std::cerr << "Unknown data type: " << dataType << std::endl;
      return;
    }

    writeDataToFile(filename, data);
  } catch (const std::exception& e) {
    std::cerr << "Error generating/saving data: " << e.what() << std::endl;
    return;
  }

  std::vector<int64_t> longData(data.begin(), data.end());

  resetDiskCounters();

  MergeSort sorter;
  Timer timer;

  timer.start();
  sorter.autoExternalSort(longData, M);
  timer.stop();

  bool isSorted = ::isSorted<int64_t>(longData);

  if (!isSorted) {
    std::string error = getSortingViolation<int64_t>(longData);
    std::cerr << "Error in MergeSort: " << error << std::endl;
  }

  std::cout << "MergeSort (" << dataType << ", n=" << size << ", M=" << M
            << "):" << std::endl;
  std::cout << "  Time: " << std::fixed << std::setprecision(6)
            << timer.getDuration() << " seconds" << std::endl;
  std::cout << "  Disk reads: " << getDiskReadCount() << std::endl;
  std::cout << "  Disk writes: " << getDiskWriteCount() << std::endl;
  std::cout << "  Correctly sorted: " << (isSorted ? "Yes" : "No") << std::endl;
  std::cout << std::endl;

  std::ofstream resultFile("data/mergesort_results.csv", std::ios::app);
  if (resultFile) {
    resultFile << dataType << "," << size << "," << M << ","
               << timer.getDuration() << "," << getDiskReadCount() << ","
               << getDiskWriteCount() << "," << (isSorted ? "1" : "0")
               << std::endl;
  }
}

int main(int argc, char* argv[]) {
  std::ifstream checkFile("data/mergesort_results.csv");
  if (!checkFile.good()) {
    std::ofstream resultFile("data/mergesort_results.csv");
    resultFile << "DataType,Size,MemorySize,Time,DiskReads,DiskWrites,Sorted"
               << std::endl;
  }

  std::vector<std::size_t> sizes = {1000, 10000, 100000};
  if (argc > 1 && std::string(argv[1]) == "large") {
    sizes.push_back(1000000);
  }

  std::vector<std::string> dataTypes = {"random", "sorted", "reverse",
                                        "partial"};
  std::vector<std::size_t> memorySizes = {100, 1000, 10000};

  for (const auto& type : dataTypes) {
    for (const auto& size : sizes) {
      for (const auto& M : memorySizes) {
        if (M > size / 10) continue;
        runMergeSortExperiment(size, type, M);
      }
    }
  }

  return 0;
}