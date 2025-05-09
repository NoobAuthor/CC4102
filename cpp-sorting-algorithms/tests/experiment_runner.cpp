#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "algorithms/mergesort.h"
#include "algorithms/quicksort.h"
#include "utils/file_handler.h"
#include "utils/sort_parameters.h"
#include "utils/timer.h"

// Run experiment for a single file
void runExperiment(const std::string& inputFile, size_t arity) {
  std::cout << "Running experiment on " << inputFile << std::endl;

  // Extract size information from filename
  std::string filename = std::filesystem::path(inputFile).filename().string();
  size_t pos1 = filename.find("_") + 1;
  size_t pos2 = filename.find("M_");
  std::string sizeStr = filename.substr(pos1, pos2 - pos1);
  size_t multiplier = std::stoul(sizeStr);

  // Load data
  std::vector<int64_t> data = readInt64DataFromFile(inputFile);

  // Results file paths
  std::string mergeResultsFile = "data/results/mergesort_results.csv";
  std::string quickResultsFile = "data/results/quicksort_results.csv";

  // Create headers if files don't exist
  if (!std::filesystem::exists(mergeResultsFile)) {
    std::ofstream file(mergeResultsFile);
    file << "Multiplier,Size,Time,DiskReads,DiskWrites,Sequence,Sorted"
         << std::endl;
  }

  if (!std::filesystem::exists(quickResultsFile)) {
    std::ofstream file(quickResultsFile);
    file << "Multiplier,Size,Time,DiskReads,DiskWrites,Sequence,Sorted"
         << std::endl;
  }

  // Extract sequence number from filename
  pos1 = filename.find("M_") + 2;
  pos2 = filename.find(".bin");
  std::string seqStr = filename.substr(pos1, pos2 - pos1);
  size_t sequence = std::stoul(seqStr);

  // Run MergeSort experiment
  {
    // Make a copy of the data
    std::vector<int64_t> testData = data;

    // Reset disk counters
    resetDiskCounters();

    // Run MergeSort
    MergeSort sorter;
    Timer timer;

    std::cout << "  Running MergeSort with arity " << arity << std::endl;
    timer.start();
    sorter.externalSort(testData, M_SIZE, arity);
    timer.stop();

    // Check if sorted
    bool sorted = true;
    for (size_t i = 1; i < testData.size(); i++) {
      if (testData[i] < testData[i - 1]) {
        sorted = false;
        break;
      }
    }

    // Save results
    std::ofstream file(mergeResultsFile, std::ios::app);
    file << multiplier << "," << data.size() << "," << timer.getDuration()
         << "," << getDiskReadCount() << "," << getDiskWriteCount() << ","
         << sequence << "," << (sorted ? "1" : "0") << std::endl;

    std::cout << "  MergeSort completed in " << timer.getDuration()
              << " seconds" << std::endl;
    std::cout << "  Disk reads: " << getDiskReadCount()
              << ", writes: " << getDiskWriteCount() << std::endl;
  }

  // Run QuickSort experiment
  {
    // Make a copy of the data
    std::vector<int64_t> testData = data;

    // Reset disk counters
    resetDiskCounters();

    // Run QuickSort
    QuickSort sorter;
    Timer timer;

    std::cout << "  Running QuickSort with arity " << arity << std::endl;
    timer.start();
    sorter.sort(testData, M_SIZE, arity);
    timer.stop();

    // Check if sorted
    bool sorted = true;
    for (size_t i = 1; i < testData.size(); i++) {
      if (testData[i] < testData[i - 1]) {
        sorted = false;
        break;
      }
    }

    // Save results
    std::ofstream file(quickResultsFile, std::ios::app);
    file << multiplier << "," << data.size() << "," << timer.getDuration()
         << "," << getDiskReadCount() << "," << getDiskWriteCount() << ","
         << sequence << "," << (sorted ? "1" : "0") << std::endl;

    std::cout << "  QuickSort completed in " << timer.getDuration()
              << " seconds" << std::endl;
    std::cout << "  Disk reads: " << getDiskReadCount()
              << ", writes: " << getDiskWriteCount() << std::endl;
  }
}

int main(int argc, char* argv[]) {
  // Create directories
  std::filesystem::create_directories("data/results");

  // Read optimal arity
  size_t arity = 0;
  std::ifstream arityFile("data/optimal_arity.txt");
  if (arityFile) {
    arityFile >> arity;
  }

  if (arity == 0) {
    std::cerr
        << "Optimal arity not found. Please run optimal_arity_finder first."
        << std::endl;
    return 1;
  }

  std::cout << "Using optimal arity: " << arity << std::endl;

  // Get all sequence files
  std::vector<std::string> seqFiles;
  for (const auto& entry :
       std::filesystem::directory_iterator("data/sequences")) {
    std::string filename = entry.path().filename().string();
    if (filename.find("seq_") == 0 && filename.size() >= 4 &&
        filename.substr(filename.size() - 4) == ".bin") {
      seqFiles.push_back(entry.path().string());
    }
  }

  if (seqFiles.empty()) {
    std::cerr << "No sequence files found. Please run data_generator first."
              << std::endl;
    return 1;
  }

  // Sort files by name for consistent processing
  std::sort(seqFiles.begin(), seqFiles.end());

  // Process files based on command line arguments
  if (argc > 1) {
    std::string sizeArg = argv[1];
    size_t targetSize = std::stoul(sizeArg);

    // Filter files for the specified size
    std::vector<std::string> filteredFiles;
    for (const auto& file : seqFiles) {
      std::string filename = std::filesystem::path(file).filename().string();
      if (filename.find("seq_" + std::to_string(targetSize) + "M_") !=
          std::string::npos) {
        filteredFiles.push_back(file);
      }
    }

    seqFiles = filteredFiles;
    std::cout << "Running experiments for size " << targetSize << "M only"
              << std::endl;
  }

  // Process each file
  for (const auto& file : seqFiles) {
    runExperiment(file, arity);
  }

  std::cout << "All experiments completed!" << std::endl;
  return 0;
}