#include "algorithms/mergesort.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <queue>

#include "utils/file_handler.h"
#include "utils/sort_parameters.h"

void MergeSort::merge(std::vector<int>& arr, int left, int mid, int right) {
  int n1 = mid - left + 1;
  int n2 = right - mid;

  std::vector<int> L(n1), R(n2);

  for (int i = 0; i < n1; i++) {
    L[i] = arr[left + i];
  }
  for (int j = 0; j < n2; j++) {
    R[j] = arr[mid + 1 + j];
  }

  int i = 0;
  int j = 0;
  int k = left;

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

  while (i < n1) {
    arr[k] = L[i];
    i++;
    k++;
  }

  while (j < n2) {
    arr[k] = R[j];
    j++;
    k++;
  }
}

void MergeSort::mergeSort(std::vector<int>& arr, int left, int right) {
  if (left < right) {
    int mid = left + (right - left) / 2;

    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);

    merge(arr, left, mid, right);
  }
}

void MergeSort::sort(std::vector<int>& arr) {
  if (!arr.empty()) {
    mergeSort(arr, 0, arr.size() - 1);
  }
}

std::vector<std::string> MergeSort::createInitialRuns(
    const std::vector<int64_t>& arr, size_t runSize,
    const std::string& tempDir) {
  std::vector<std::string> runFiles;
  size_t n = arr.size();
  size_t runCount = (n + runSize - 1) / runSize;

  for (size_t i = 0; i < runCount; i++) {
    size_t startIdx = i * runSize;
    size_t endIdx = std::min(startIdx + runSize, n);

    std::vector<int64_t> run(arr.begin() + startIdx, arr.begin() + endIdx);

    std::sort(run.begin(), run.end());

    std::string runFile = tempDir + "/run_" + std::to_string(i) + ".bin";
    writeInt64DataToFile(run, runFile);

    runFiles.push_back(runFile);
  }

  return runFiles;
}

struct HeapNode {
  int64_t value;
  size_t runIndex;
  size_t posInRun;

  bool operator>(const HeapNode& other) const { return value > other.value; }
};

void MergeSort::mergeRuns(const std::vector<std::string>& runFiles,
                          const std::string& outputFile, size_t M, size_t a) {
  size_t K = runFiles.size();

  if (K == 0) return;
  if (K == 1) {
    std::filesystem::copy_file(
        runFiles[0], outputFile,
        std::filesystem::copy_options::overwrite_existing);
    return;
  }

  size_t bufferSize = calculateOptimalBufferSize(M, K, a);

  size_t mergeAtOnce = std::min(K, a);

  std::cout << "Merging " << K << " runs using " << mergeAtOnce
            << "-way merge with buffer size " << bufferSize << std::endl;

  if (K > mergeAtOnce) {
    std::vector<std::string> intermediateRunFiles;

    for (size_t i = 0; i < K; i += mergeAtOnce) {
      size_t endIdx = std::min(i + mergeAtOnce, K);
      std::vector<std::string> batchRuns(runFiles.begin() + i,
                                         runFiles.begin() + endIdx);

      std::string intermediateFile = runFiles[0] + ".intermediate_" +
                                     std::to_string(i / mergeAtOnce) + ".bin";
      mergeRuns(batchRuns, intermediateFile, M, mergeAtOnce);
      intermediateRunFiles.push_back(intermediateFile);
    }

    mergeRuns(intermediateRunFiles, outputFile, M, a);

    for (const auto& file : intermediateRunFiles) {
      std::filesystem::remove(file);
    }

    return;
  }

  std::vector<std::ifstream> runStreams(K);
  for (size_t i = 0; i < K; i++) {
    runStreams[i].open(runFiles[i], std::ios::binary);
    if (!runStreams[i].is_open()) {
      throw std::runtime_error("Could not open run file: " + runFiles[i]);
    }
  }

  std::ofstream outStream(outputFile, std::ios::binary);
  if (!outStream.is_open()) {
    throw std::runtime_error("Could not create output file: " + outputFile);
  }

  std::vector<std::vector<int64_t>> buffers(K);
  std::vector<size_t> bufferPos(K, 0);
  std::vector<bool> runExhausted(K, false);

  for (size_t i = 0; i < K; i++) {
    buffers[i].resize(bufferSize);

    runStreams[i].read(reinterpret_cast<char*>(buffers[i].data()),
                       bufferSize * sizeof(int64_t));
    size_t elementsRead = runStreams[i].gcount() / sizeof(int64_t);
    buffers[i].resize(elementsRead);

    if (elementsRead == 0) {
      runExhausted[i] = true;
    }

    disk_read_count++;
  }

  std::priority_queue<HeapNode, std::vector<HeapNode>, std::greater<HeapNode>>
      minHeap;

  for (size_t i = 0; i < K; i++) {
    if (!runExhausted[i]) {
      minHeap.push({buffers[i][0], i, 0});
    }
  }

  std::vector<int64_t> outputBuffer;
  outputBuffer.reserve(bufferSize);

  while (!minHeap.empty()) {
    HeapNode top = minHeap.top();
    minHeap.pop();

    outputBuffer.push_back(top.value);

    if (outputBuffer.size() >= bufferSize) {
      outStream.write(reinterpret_cast<const char*>(outputBuffer.data()),
                      outputBuffer.size() * sizeof(int64_t));
      disk_write_count++;
      outputBuffer.clear();
    }

    size_t runIdx = top.runIndex;
    size_t posInRun = top.posInRun + 1;

    if (posInRun >= buffers[runIdx].size()) {
      buffers[runIdx].resize(bufferSize);
      bufferPos[runIdx] = 0;

      runStreams[runIdx].read(reinterpret_cast<char*>(buffers[runIdx].data()),
                              bufferSize * sizeof(int64_t));
      size_t elementsRead = runStreams[runIdx].gcount() / sizeof(int64_t);

      if (elementsRead == 0) {
        runExhausted[runIdx] = true;
      } else {
        buffers[runIdx].resize(elementsRead);

        minHeap.push({buffers[runIdx][0], runIdx, 0});

        disk_read_count++;
      }
    } else {
      minHeap.push({buffers[runIdx][posInRun], runIdx, posInRun});
    }
  }

  if (!outputBuffer.empty()) {
    outStream.write(reinterpret_cast<const char*>(outputBuffer.data()),
                    outputBuffer.size() * sizeof(int64_t));
    disk_write_count++;
  }

  for (auto& stream : runStreams) {
    stream.close();
  }
  outStream.close();
}

void MergeSort::mergeSortedRuns(const std::vector<std::string>& runFiles,
                                std::vector<int64_t>& output, size_t M,
                                size_t a) {
  std::string outputFile = "data/mergesort_temp/final_output.bin";

  mergeRuns(runFiles, outputFile, M, a);

  output = readInt64DataFromFile(outputFile);

  std::filesystem::remove(outputFile);
}

void MergeSort::externalSort(std::vector<int64_t>& arr, size_t M, size_t a) {
  if (arr.empty()) return;

  std::cout << "Running external mergesort with M=" << M << ", a=" << a
            << " for " << arr.size() << " elements" << std::endl;

  resetDiskCounters();

  try {
    std::string tempDir = "data/mergesort_temp";
    std::filesystem::create_directories(tempDir);

    size_t runSize = M / 2;
    if (runSize == 0) runSize = 1;

    std::vector<std::string> runFiles =
        createInitialRuns(arr, runSize, tempDir);
    std::cout << "Created " << runFiles.size() << " initial runs" << std::endl;

    mergeSortedRuns(runFiles, arr, M, a);

    for (const auto& file : runFiles) {
      std::filesystem::remove(file);
    }

  } catch (const std::exception& e) {
    std::cerr << "Error in external mergesort: " << e.what() << std::endl;

    std::cerr << "Falling back to in-memory sort" << std::endl;
    std::sort(arr.begin(), arr.end());
  }
}

void MergeSort::autoExternalSort(std::vector<int64_t>& arr, size_t M) {
  size_t elementSize = sizeof(int64_t);
  size_t blockSize = 4096;

  size_t bufferElements = blockSize / elementSize;

  size_t optimalArity = calculateOptimalArity(M, bufferElements);

  std::cout << "Auto-tuned parameters: M=" << M
            << ", calculated optimal arity=" << optimalArity << std::endl;

  externalSort(arr, M, optimalArity);
}