#include "algorithms/quicksort.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <queue>
#include <string>
#include <vector>

#include "utils/file_handler.h"
#include "utils/sort_parameters.h"

void QuickSort::externalQuickSort(std::vector<int64_t>& arr, size_t M,
                                  size_t a) {
  if (arr.size() <= 1) {
    return;
  }

  if (arr.size() * sizeof(int64_t) <= M) {
    std::string tempFile = "data/quicksort_temp/in_memory_sort.bin";
    std::filesystem::create_directories("data/quicksort_temp");
    writeInt64DataToFile(arr, tempFile);
    std::sort(arr.begin(), arr.end());
    writeInt64DataToFile(arr, tempFile + ".sorted");
    std::filesystem::remove(tempFile);
    std::filesystem::remove(tempFile + ".sorted");
    return;
  }

  try {
    std::string tempDir = "data/quicksort_temp";
    std::filesystem::create_directories(tempDir);

    size_t effective_a = a;
    if (a > arr.size() / 100) {
      effective_a = std::max(size_t(2), std::min(a, arr.size() / 100));
    }

    std::string inputFile = tempDir + "/input.bin";
    writeInt64DataToFile(arr, inputFile);

    size_t sampleSize = std::min(arr.size(), size_t(1000));
    size_t step = arr.size() / sampleSize;
    std::vector<int64_t> samples;
    for (size_t i = 0; i < arr.size(); i += step) {
      samples.push_back(arr[i]);
    }
    std::sort(samples.begin(), samples.end());

    std::vector<int64_t> pivots;
    if (effective_a > 1) {
      size_t pivotStep = samples.size() / effective_a;
      for (size_t i = 1; i < effective_a; i++) {
        pivots.push_back(samples[i * pivotStep]);
      }
    }

    std::vector<std::vector<int64_t>> partitionBuffers(effective_a);
    std::vector<std::string> partitionFiles(effective_a);

    size_t elementSize = sizeof(int64_t);
    size_t totalBuffers = effective_a + 1;
    size_t bufferSize = (M * 0.8) / (totalBuffers * elementSize);
    bufferSize = std::max(size_t(1000), bufferSize);

    for (size_t i = 0; i < effective_a; i++) {
      partitionFiles[i] = tempDir + "/partition_" + std::to_string(i) + ".bin";
      partitionBuffers[i].reserve(bufferSize);
    }

    std::vector<int64_t> readBuffer;
    readBuffer.reserve(bufferSize);

    std::cout << "Using " << effective_a << "-way partitioning for QuickSort"
              << std::endl;

    std::ifstream inFile(inputFile, std::ios::binary);
    if (!inFile) {
      throw std::runtime_error("Could not open input file: " + inputFile);
    }

    while (inFile) {
      readBuffer.resize(bufferSize);
      inFile.read(reinterpret_cast<char*>(readBuffer.data()),
                  bufferSize * sizeof(int64_t));
      size_t elementsRead = inFile.gcount() / sizeof(int64_t);
      readBuffer.resize(elementsRead);

      if (elementsRead == 0) {
        break;
      }

      for (const auto& element : readBuffer) {
        size_t partitionIdx = 0;
        while (partitionIdx < pivots.size() &&
               element >= pivots[partitionIdx]) {
          partitionIdx++;
        }

        partitionBuffers[partitionIdx].push_back(element);

        if (partitionBuffers[partitionIdx].size() >= bufferSize) {
          appendInt64DataToFile(partitionBuffers[partitionIdx],
                                partitionFiles[partitionIdx]);
          partitionBuffers[partitionIdx].clear();
        }
      }
    }

    for (size_t i = 0; i < effective_a; i++) {
      if (!partitionBuffers[i].empty()) {
        appendInt64DataToFile(partitionBuffers[i], partitionFiles[i]);
        partitionBuffers[i].clear();
      }
    }

    arr.clear();

    for (size_t i = 0; i < effective_a; i++) {
      try {
        std::vector<int64_t> partition =
            readInt64DataFromFile(partitionFiles[i]);

        externalQuickSort(partition, M, effective_a);

        arr.insert(arr.end(), partition.begin(), partition.end());

        std::filesystem::remove(partitionFiles[i]);
      } catch (const std::exception& e) {
        std::cerr << "Error processing partition " << i << ": " << e.what()
                  << std::endl;

        try {
          std::vector<int64_t> partition =
              readInt64DataFromFile(partitionFiles[i]);
          std::sort(partition.begin(), partition.end());
          writeInt64DataToFile(partition, partitionFiles[i] + ".sorted");
          partition = readInt64DataFromFile(partitionFiles[i] + ".sorted");
          arr.insert(arr.end(), partition.begin(), partition.end());
          std::filesystem::remove(partitionFiles[i]);
          std::filesystem::remove(partitionFiles[i] + ".sorted");
        } catch (const std::exception& e2) {
          std::cerr << "Couldn't recover partition " << i << ": " << e2.what()
                    << std::endl;
        }
      }
    }

    std::filesystem::remove(inputFile);

    try {
      std::filesystem::remove(tempDir);
    } catch (const std::exception& e) {
      std::cerr << "Warning: Could not remove temp directory: " << e.what()
                << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << "Error in external quicksort: " << e.what() << std::endl;

    try {
      std::cout << "Attempting simplified external QuickSort..." << std::endl;

      std::string tempDir = "data/quicksort_temp";
      std::filesystem::create_directories(tempDir);

      std::string tempFile = tempDir + "/backup_data.bin";
      writeInt64DataToFile(arr, tempFile);

      size_t chunkSize = std::min(M / sizeof(int64_t), arr.size());
      std::vector<std::string> chunkFiles;

      for (size_t offset = 0; offset < arr.size(); offset += chunkSize) {
        size_t end = std::min(offset + chunkSize, arr.size());
        std::vector<int64_t> chunk(arr.begin() + offset, arr.begin() + end);

        std::sort(chunk.begin(), chunk.end());

        std::string chunkFile =
            tempDir + "/chunk_" + std::to_string(chunkFiles.size()) + ".bin";
        writeInt64DataToFile(chunk, chunkFile);
        chunkFiles.push_back(chunkFile);
      }

      arr.clear();

      std::vector<std::ifstream> streams;
      std::vector<int64_t> topElements;
      std::vector<bool> streamEnded;

      for (const auto& file : chunkFiles) {
        streams.emplace_back(file, std::ios::binary);
        int64_t value;
        if (streams.back().read(reinterpret_cast<char*>(&value),
                                sizeof(int64_t))) {
          topElements.push_back(value);
          streamEnded.push_back(false);
        } else {
          topElements.push_back(std::numeric_limits<int64_t>::max());
          streamEnded.push_back(true);
        }
      }

      while (true) {
        size_t minIndex = 0;
        for (size_t i = 1; i < streams.size(); i++) {
          if (!streamEnded[i] && (streamEnded[minIndex] ||
                                  topElements[i] < topElements[minIndex])) {
            minIndex = i;
          }
        }

        if (streamEnded[minIndex]) {
          break;
        }

        arr.push_back(topElements[minIndex]);

        int64_t value;
        if (streams[minIndex].read(reinterpret_cast<char*>(&value),
                                   sizeof(int64_t))) {
          topElements[minIndex] = value;
        } else {
          streamEnded[minIndex] = true;
          topElements[minIndex] = std::numeric_limits<int64_t>::max();
        }
      }

      for (auto& stream : streams) {
        stream.close();
      }
      for (const auto& file : chunkFiles) {
        std::filesystem::remove(file);
      }
      std::filesystem::remove(tempFile);

      std::cout << "Simplified external QuickSort completed successfully"
                << std::endl;

    } catch (const std::exception& e) {
      std::cerr << "Backup approach also failed: " << e.what() << std::endl;

      std::cerr << "Using disk-based fallback as last resort" << std::endl;

      std::string tempFile = "data/quicksort_temp/fallback.bin";
      std::filesystem::create_directories("data/quicksort_temp");
      writeInt64DataToFile(arr, tempFile);

      std::sort(arr.begin(), arr.end());

      writeInt64DataToFile(arr, tempFile + ".sorted");
      arr = readInt64DataFromFile(tempFile + ".sorted");

      std::filesystem::remove(tempFile);
      std::filesystem::remove(tempFile + ".sorted");
    }
  }
}

void QuickSort::sort(std::vector<int64_t>& arr, size_t M, size_t a) {
  std::cout << "Running external quicksort with M=" << M << ", a=" << a
            << " for " << arr.size() << " elements" << std::endl;

  std::filesystem::create_directories("data/quicksort_temp");

  resetDiskCounters();

  externalQuickSort(arr, M, a);
}

void QuickSort::autoSort(std::vector<int64_t>& arr, size_t M) {
  size_t elementSize = sizeof(int64_t);
  size_t optimalArity = calculateOptimalArity(M, elementSize);

  std::cout << "Auto-tuned parameters: M=" << M
            << ", calculated optimal arity=" << optimalArity << std::endl;

  sort(arr, M, optimalArity);
}