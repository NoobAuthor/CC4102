#include "algorithms/quicksort.h"

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <random>
#include <string>

#include "utils/file_handler.h"
#include "utils/sort_parameters.h"

void QuickSort::partition(std::vector<int64_t>& arr,
                          std::vector<int64_t>& pivots,
                          std::vector<std::vector<int64_t>>& subarrays) {
  // Initialize subarrays (a = pivots.size() + 1 subarrays)
  subarrays.resize(pivots.size() + 1);

  // Partition the array according to the pivots
  for (const auto& element : arr) {
    // Find the correct subarray for the element
    size_t i = 0;
    while (i < pivots.size() && element > pivots[i]) {
      i++;
    }
    // Add element to the appropriate subarray
    subarrays[i].push_back(element);
  }
}

void QuickSort::externalQuickSort(std::vector<int64_t>& arr, size_t M,
                                  size_t a) {
  // Base case: if array size is small enough, sort in memory
  if (arr.size() <= M) {
    std::sort(arr.begin(), arr.end());
    return;
  }

  try {
    // For large data and many subarrays, limit the number of subarrays
    // to prevent creating too many files
    size_t effective_a = std::min(a, static_cast<size_t>(16));

    // Create a temporary directory for this recursive call
    static int callCount = 0;
    std::string tempDir = "data/quicksort_temp_" + std::to_string(callCount++);

    // Make sure the directory exists
    std::filesystem::create_directories(tempDir);

    // Write the input array to disk
    std::string inputFile = tempDir + "/input.bin";
    writeInt64DataToFile(arr, inputFile);

    // Select effective_a-1 pivots randomly from the array
    std::vector<int64_t> pivots;

    // Select random indices for pivots
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, arr.size() - 1);

    // Sample from the array to select pivots
    std::vector<int64_t> sample;
    size_t sampleSize = std::min(arr.size(), M / 2);
    for (size_t i = 0; i < sampleSize; i++) {
      sample.push_back(arr[distrib(gen)]);
    }

    // Sort the sample
    std::sort(sample.begin(), sample.end());

    // Select evenly spaced elements as pivots
    for (size_t i = 1; i < effective_a; i++) {
      size_t index = (i * sample.size()) / effective_a;
      if (index < sample.size()) {
        pivots.push_back(sample[index]);
      }
    }

    // Sort the pivots
    std::sort(pivots.begin(), pivots.end());

    // Remove duplicates
    pivots.erase(std::unique(pivots.begin(), pivots.end()), pivots.end());

    // Adjust effective_a based on the number of unique pivots
    effective_a = pivots.size() + 1;

    std::cout << "Using " << effective_a << "-way partitioning for QuickSort"
              << std::endl;

    // Create partition files
    std::vector<std::string> partitionFiles;
    for (size_t i = 0; i < effective_a; i++) {
      partitionFiles.push_back(tempDir + "/partition_" + std::to_string(i) +
                               ".bin");
    }

    // Calculate buffer size based on memory and arity
    size_t bufferSize = calculateOptimalBufferSize(M, arr.size(), effective_a);
    size_t blockSize = bufferSize * sizeof(int64_t);  // Block size in bytes

    // Process the input file in blocks
    size_t fileSize = arr.size() * sizeof(int64_t);

    std::vector<std::vector<int64_t>> partitionBuffers(effective_a);

    for (size_t offset = 0; offset < fileSize; offset += blockSize) {
      // Read a block from the input file
      std::vector<int64_t> block =
          readBlockFromFile(inputFile, offset, blockSize);

      // Partition the block
      for (const auto& element : block) {
        // Find the correct partition for this element
        size_t i = 0;
        while (i < pivots.size() && element > pivots[i]) {
          i++;
        }
        // Add to partition buffer
        partitionBuffers[i].push_back(element);

        // If a buffer gets too big, write it to disk
        if (partitionBuffers[i].size() >= bufferSize) {
          appendInt64DataToFile(partitionBuffers[i], partitionFiles[i]);
          partitionBuffers[i].clear();
        }
      }
    }

    // Write any remaining buffer data to disk
    for (size_t i = 0; i < effective_a; i++) {
      if (!partitionBuffers[i].empty()) {
        appendInt64DataToFile(partitionBuffers[i], partitionFiles[i]);
        partitionBuffers[i].clear();
      }
    }

    // Recursively sort each partition
    arr.clear();  // Clear original array

    for (size_t i = 0; i < effective_a; i++) {
      try {
        // Read partition from disk
        std::vector<int64_t> partition =
            readInt64DataFromFile(partitionFiles[i]);

        // Sort the partition
        externalQuickSort(partition, M, effective_a);

        // Append sorted partition to result
        arr.insert(arr.end(), partition.begin(), partition.end());

        // Clean up partition file
        std::filesystem::remove(partitionFiles[i]);
      } catch (const std::exception& e) {
        std::cerr << "Error processing partition " << i << ": " << e.what()
                  << std::endl;
      }
    }

    // Clean up temporary input file
    std::filesystem::remove(inputFile);

    // Clean up temporary directory
    try {
      std::filesystem::remove(tempDir);
    } catch (const std::exception& e) {
      // Directory might not be empty if some operations failed
      std::cerr << "Warning: Could not remove temp directory: " << e.what()
                << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << "Error in external quicksort: " << e.what() << std::endl;
    // Fall back to in-memory sort
    std::cerr << "Falling back to in-memory sort" << std::endl;
    std::sort(arr.begin(), arr.end());
  }
}

// Public method to start the sorting process with specified parameters
void QuickSort::sort(std::vector<int64_t>& arr, size_t M, size_t a) {
  std::cout << "Running external quicksort with M=" << M << ", a=" << a
            << " for " << arr.size() << " elements" << std::endl;

  // Create base data directory if it doesn't exist
  std::filesystem::create_directories("data/quicksort_temp");

  // Reset disk counters
  resetDiskCounters();

  // Call the external quicksort algorithm
  externalQuickSort(arr, M, a);
}

// Auto-tuned version of QuickSort
void QuickSort::autoSort(std::vector<int64_t>& arr, size_t M) {
  // Calculate optimal arity based on M and estimated buffer size
  size_t elementSize = sizeof(int64_t);
  size_t blockSize = 4096;  // Typical filesystem block size

  // We want buffers that are multiple of block size for efficient I/O
  size_t bufferElements = blockSize / elementSize;

  // Calculate optimal arity
  size_t optimalArity = calculateOptimalArity(M, bufferElements);

  std::cout << "Auto-tuned parameters: M=" << M
            << ", calculated optimal arity=" << optimalArity << std::endl;

  // Run with calculated optimal parameters
  sort(arr, M, optimalArity);
}