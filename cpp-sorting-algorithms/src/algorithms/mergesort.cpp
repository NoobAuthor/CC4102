#include "algorithms/mergesort.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>

#include "utils/file_handler.h"
#include "utils/sort_parameters.h"

// In-memory merge function
void MergeSort::merge(std::vector<int>& arr, int left, int mid, int right) {
  int n1 = mid - left + 1;
  int n2 = right - mid;

  // Create temporary arrays
  std::vector<int> L(n1), R(n2);

  // Copy data to temp arrays
  for (int i = 0; i < n1; i++) {
    L[i] = arr[left + i];
  }
  for (int j = 0; j < n2; j++) {
    R[j] = arr[mid + 1 + j];
  }

  // Merge the temp arrays back into arr[left..right]
  int i = 0;     // Initial index of first subarray
  int j = 0;     // Initial index of second subarray
  int k = left;  // Initial index of merged subarray

  while (i < n1 && j < n2) {
    if (L[i] <= R[j]) {
      arr[k] = L[i];
      i++;
    } else {
      arr[k] = R[j];
      j++;
    }
    k++;
  }

  // Copy the remaining elements of L[]
  while (i < n1) {
    arr[k] = L[i];
    i++;
    k++;
  }

  // Copy the remaining elements of R[]
  while (j < n2) {
    arr[k] = R[j];
    j++;
    k++;
  }
}

// Recursive in-memory mergesort
void MergeSort::mergeSort(std::vector<int>& arr, int left, int right) {
  if (left < right) {
    int mid = left + (right - left) / 2;

    // Sort first and second halves
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);

    // Merge the sorted halves
    merge(arr, left, mid, right);
  }
}

// Public method for in-memory sort
void MergeSort::sort(std::vector<int>& arr) {
  if (!arr.empty()) {
    mergeSort(arr, 0, arr.size() - 1);
  }
}

// Create initial sorted runs for external sorting
std::vector<std::string> MergeSort::createInitialRuns(
    const std::vector<int64_t>& arr, size_t runSize,
    const std::string& tempDir) {
  std::vector<std::string> runFiles;
  size_t n = arr.size();
  size_t runCount = (n + runSize - 1) / runSize;  // Ceiling division

  for (size_t i = 0; i < runCount; i++) {
    // Create a run with at most runSize elements
    size_t startIdx = i * runSize;
    size_t endIdx = std::min(startIdx + runSize, n);

    // Copy elements for this run
    std::vector<int64_t> run(arr.begin() + startIdx, arr.begin() + endIdx);

    // Sort the run in memory
    std::sort(run.begin(), run.end());

    // Write the sorted run to a file
    std::string runFile = tempDir + "/run_" + std::to_string(i) + ".bin";
    writeInt64DataToFile(run, runFile);

    runFiles.push_back(runFile);
  }

  return runFiles;
}

// Helper structure for K-way merge
struct HeapNode {
  int64_t value;    // Value at current position
  size_t runIndex;  // Index of the run from which this value came
  size_t posInRun;  // Current position in the run

  // Comparison operator for min heap
  bool operator>(const HeapNode& other) const { return value > other.value; }
};

// Merge K sorted runs using K-way merge
void MergeSort::mergeRuns(const std::vector<std::string>& runFiles,
                          const std::string& outputFile, size_t M, size_t a) {
  // Number of runs
  size_t K = runFiles.size();

  if (K == 0) return;
  if (K == 1) {
    // If only one run, it's already sorted
    std::filesystem::copy_file(
        runFiles[0], outputFile,
        std::filesystem::copy_options::overwrite_existing);
    return;
  }

  // Calculate buffer size based on memory and arity
  // We need a input buffers + 1 output buffer, so buffer size = M/(a+1)
  size_t bufferSize = calculateOptimalBufferSize(M, K, a);

  // For a-way merge, we'll merge at most a runs at once
  size_t mergeAtOnce = std::min(K, a);

  std::cout << "Merging " << K << " runs using " << mergeAtOnce
            << "-way merge with buffer size " << bufferSize << std::endl;

  // If we have more runs than our arity, use multi-level merging
  if (K > mergeAtOnce) {
    std::vector<std::string> intermediateRunFiles;

    for (size_t i = 0; i < K; i += mergeAtOnce) {
      // Take next batch of runs
      size_t endIdx = std::min(i + mergeAtOnce, K);
      std::vector<std::string> batchRuns(runFiles.begin() + i,
                                         runFiles.begin() + endIdx);

      // Merge this batch
      std::string intermediateFile = runFiles[0] + ".intermediate_" +
                                     std::to_string(i / mergeAtOnce) + ".bin";
      mergeRuns(batchRuns, intermediateFile, M, mergeAtOnce);
      intermediateRunFiles.push_back(intermediateFile);
    }

    // Merge the intermediate runs recursively
    mergeRuns(intermediateRunFiles, outputFile, M, a);

    // Clean up intermediate files
    for (const auto& file : intermediateRunFiles) {
      std::filesystem::remove(file);
    }

    return;
  }

  // Open all run files
  std::vector<std::ifstream> runStreams(K);
  for (size_t i = 0; i < K; i++) {
    runStreams[i].open(runFiles[i], std::ios::binary);
    if (!runStreams[i].is_open()) {
      throw std::runtime_error("Could not open run file: " + runFiles[i]);
    }
  }

  // Create output file
  std::ofstream outStream(outputFile, std::ios::binary);
  if (!outStream.is_open()) {
    throw std::runtime_error("Could not create output file: " + outputFile);
  }

  // Create buffers for each run
  std::vector<std::vector<int64_t>> buffers(K);
  std::vector<size_t> bufferPos(K, 0);  // Current position in each buffer
  std::vector<bool> runExhausted(K, false);

  // Initialize buffers
  for (size_t i = 0; i < K; i++) {
    buffers[i].resize(bufferSize);

    // Read initial data into buffer
    runStreams[i].read(reinterpret_cast<char*>(buffers[i].data()),
                       bufferSize * sizeof(int64_t));
    size_t elementsRead = runStreams[i].gcount() / sizeof(int64_t);
    buffers[i].resize(elementsRead);

    if (elementsRead == 0) {
      runExhausted[i] = true;
    }

    // Increment disk read counter
    disk_read_count++;
  }

  // Create min heap for K-way merge
  std::priority_queue<HeapNode, std::vector<HeapNode>, std::greater<HeapNode>>
      minHeap;

  // Initialize heap with first element from each run
  for (size_t i = 0; i < K; i++) {
    if (!runExhausted[i]) {
      minHeap.push({buffers[i][0], i, 0});
    }
  }

  // Output buffer
  std::vector<int64_t> outputBuffer;
  outputBuffer.reserve(bufferSize);

  // Perform K-way merge
  while (!minHeap.empty()) {
    // Get smallest element
    HeapNode top = minHeap.top();
    minHeap.pop();

    // Add to output buffer
    outputBuffer.push_back(top.value);

    // If output buffer is full, write to disk
    if (outputBuffer.size() >= bufferSize) {
      outStream.write(reinterpret_cast<const char*>(outputBuffer.data()),
                      outputBuffer.size() * sizeof(int64_t));
      disk_write_count++;
      outputBuffer.clear();
    }

    // Advance in the run that provided the top element
    size_t runIdx = top.runIndex;
    size_t posInRun = top.posInRun + 1;

    // If we've exhausted the current buffer, read more from file
    if (posInRun >= buffers[runIdx].size()) {
      buffers[runIdx].resize(bufferSize);
      bufferPos[runIdx] = 0;

      runStreams[runIdx].read(reinterpret_cast<char*>(buffers[runIdx].data()),
                              bufferSize * sizeof(int64_t));
      size_t elementsRead = runStreams[runIdx].gcount() / sizeof(int64_t);

      if (elementsRead == 0) {
        // This run is exhausted
        runExhausted[runIdx] = true;
      } else {
        // Resize buffer to actual elements read
        buffers[runIdx].resize(elementsRead);

        // Add first element from new buffer to heap
        minHeap.push({buffers[runIdx][0], runIdx, 0});

        // Increment disk read counter
        disk_read_count++;
      }
    } else {
      // Add next element from the same buffer to heap
      minHeap.push({buffers[runIdx][posInRun], runIdx, posInRun});
    }
  }

  // Write any remaining elements in the output buffer
  if (!outputBuffer.empty()) {
    outStream.write(reinterpret_cast<const char*>(outputBuffer.data()),
                    outputBuffer.size() * sizeof(int64_t));
    disk_write_count++;
  }

  // Close all files
  for (auto& stream : runStreams) {
    stream.close();
  }
  outStream.close();
}

// Build a single merged output from multiple runs
void MergeSort::mergeSortedRuns(const std::vector<std::string>& runFiles,
                                std::vector<int64_t>& output, size_t M,
                                size_t a) {
  // Create temp output file
  std::string outputFile = "data/mergesort_temp/final_output.bin";

  // Merge all run files into the output file
  mergeRuns(runFiles, outputFile, M, a);

  // Read the sorted output back into memory
  output = readInt64DataFromFile(outputFile);

  // Clean up the output file
  std::filesystem::remove(outputFile);
}

// External mergesort with specified arity
void MergeSort::externalSort(std::vector<int64_t>& arr, size_t M, size_t a) {
  if (arr.empty()) return;

  std::cout << "Running external mergesort with M=" << M << ", a=" << a
            << " for " << arr.size() << " elements" << std::endl;

  // Reset disk counters
  resetDiskCounters();

  try {
    // Create temporary directory
    std::string tempDir = "data/mergesort_temp";
    std::filesystem::create_directories(tempDir);

    // Calculate buffer size based on memory and arity
    size_t bufferSize = calculateOptimalBufferSize(M, arr.size(), a);

    // Phase 1: Create initial sorted runs
    size_t runSize = M / 2;         // Use half of memory for each run
    if (runSize == 0) runSize = 1;  // Ensure at least 1 element per run

    std::vector<std::string> runFiles =
        createInitialRuns(arr, runSize, tempDir);
    std::cout << "Created " << runFiles.size() << " initial runs" << std::endl;

    // Phase 2: Merge the runs
    mergeSortedRuns(runFiles, arr, M, a);

    // Clean up the run files
    for (const auto& file : runFiles) {
      std::filesystem::remove(file);
    }

  } catch (const std::exception& e) {
    std::cerr << "Error in external mergesort: " << e.what() << std::endl;

    // Fall back to in-memory sort if external sort fails
    std::cerr << "Falling back to in-memory sort" << std::endl;
    std::sort(arr.begin(), arr.end());
  }
}

// Auto-tuned external sort
void MergeSort::autoExternalSort(std::vector<int64_t>& arr, size_t M) {
  // Calculate optimal arity based on M and estimated buffer size
  size_t elementSize = sizeof(int64_t);
  size_t blockSize = 4096;  // Typical filesystem block size

  // We want buffers that are multiple of block size for efficient I/O
  size_t bufferElements = blockSize / elementSize;

  // Calculate optimal arity
  size_t optimalArity = calculateOptimalArity(M, bufferElements);

  std::cout << "Auto-tuned parameters: M=" << M
            << ", calculated optimal arity=" << optimalArity << std::endl;

  // Run external sort with calculated optimal arity
  externalSort(arr, M, optimalArity);
}