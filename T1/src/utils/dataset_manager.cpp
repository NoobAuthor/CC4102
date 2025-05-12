#include "utils/dataset_manager.h"

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>

#include "utils/file_handler.h"
#include "utils/sort_parameters.h"

DatasetManager::DatasetManager(unsigned int seed)
    : _seed(seed), _useRandomSeed(seed == 0) {}

std::vector<int> DatasetManager::generateIntData(std::size_t size,
                                                 const std::string& type,
                                                 double sortedFraction) {
  std::vector<int> data(size);

  std::random_device rd;
  std::mt19937 gen(_useRandomSeed ? rd() : _seed);

  if (type == "random") {
    std::uniform_int_distribution<int> distrib(0, 1000000);

    for (std::size_t i = 0; i < size; i++) {
      data[i] = distrib(gen);
    }
  } else if (type == "sorted") {
    for (std::size_t i = 0; i < size; i++) {
      data[i] = static_cast<int>(i);
    }
  } else if (type == "reverse_sorted") {
    for (std::size_t i = 0; i < size; i++) {
      data[i] = static_cast<int>(size - i - 1);
    }
  } else if (type == "partially_sorted") {
    for (std::size_t i = 0; i < size; i++) {
      data[i] = static_cast<int>(i);
    }

    std::size_t shuffleCount =
        static_cast<std::size_t>(size * (1.0 - sortedFraction));

    for (std::size_t i = 0; i < shuffleCount; i++) {
      std::uniform_int_distribution<std::size_t> distrib(0, size - 1);
      std::size_t pos1 = distrib(gen);
      std::size_t pos2 = distrib(gen);
      std::swap(data[pos1], data[pos2]);
    }
  } else {
    std::cerr << "Unknown data type '" << type
              << "'. Using random data instead." << std::endl;
    return generateIntData(size, "random", sortedFraction);
  }

  return data;
}

std::vector<int64_t> DatasetManager::generateInt64Data(std::size_t size,
                                                       const std::string& type,
                                                       double sortedFraction) {
  if (type != "random") {
    std::vector<int> intData = generateIntData(size, type, sortedFraction);
    std::vector<int64_t> data(size);
    std::transform(intData.begin(), intData.end(), data.begin(),
                   [](int x) { return static_cast<int64_t>(x); });
    return data;
  }

  std::vector<int64_t> data(size);

  std::random_device rd;
  std::mt19937_64 gen(_useRandomSeed ? rd() : _seed);

  std::uniform_int_distribution<int64_t> distrib(INT64_MIN, INT64_MAX);

  for (std::size_t i = 0; i < size; i++) {
    data[i] = distrib(gen);
  }

  return data;
}

void DatasetManager::generateAndSaveInt64Sequence(std::size_t size,
                                                  const std::string& filename) {
  std::cout << "Generating data sequence of size " << size << " ("
            << (size * sizeof(int64_t) / (1024 * 1024)) << "MB)" << std::endl;

  std::vector<int64_t> data = generateInt64Data(size);

  writeInt64DataToFile(data, filename);
  std::cout << "Wrote " << data.size() << " elements to " << filename
            << std::endl;
}

bool DatasetManager::checkAllDatasetsExist(std::size_t maxMultiplier) {
  for (size_t multiplier = 4; multiplier <= maxMultiplier; multiplier += 4) {
    for (int sequence = 1; sequence <= 5; sequence++) {
      std::string filename = "data/sequences/seq_" +
                             std::to_string(multiplier) + "M_" +
                             std::to_string(sequence) + ".bin";
      if (!std::filesystem::exists(filename)) {
        return false;
      }
    }
  }

  std::string arityTestFile =
      "data/sequences/arity_test_" + std::to_string(maxMultiplier) + "M.bin";
  if (!std::filesystem::exists(arityTestFile)) {
    return false;
  }

  return true;
}

void DatasetManager::generateExperimentDatasets(std::size_t maxMultiplier) {
  std::filesystem::create_directories("data/sequences");

  const size_t M_ELEMENTS = mbToElementCount(50);

  std::cout << "Generating sequences up to " << maxMultiplier << "M"
            << std::endl;

  for (size_t multiplier = 4; multiplier <= maxMultiplier; multiplier += 4) {
    size_t N = multiplier * M_ELEMENTS;

    std::cout << "Generating 5 sequences of size " << multiplier << "M (" << N
              << " elements)" << std::endl;

    for (int sequence = 1; sequence <= 5; sequence++) {
      std::string filename = "data/sequences/seq_" +
                             std::to_string(multiplier) + "M_" +
                             std::to_string(sequence) + ".bin";
      generateAndSaveInt64Sequence(N, filename);
    }
  }

  std::string arityTestFile =
      "data/sequences/arity_test_" + std::to_string(maxMultiplier) + "M.bin";
  size_t N = maxMultiplier * M_ELEMENTS;
  generateAndSaveInt64Sequence(N, arityTestFile);

  std::cout << "All sequences generated successfully!" << std::endl;
}

std::vector<int> DatasetManager::generateRandomData(std::size_t size) {
  DatasetManager manager;
  return manager.generateIntData(size, "random");
}

std::vector<int64_t> DatasetManager::generateRandomInt64Data(std::size_t size) {
  DatasetManager manager;
  return manager.generateInt64Data(size, "random");
}

std::vector<int> DatasetManager::generateSortedData(std::size_t size) {
  DatasetManager manager;
  return manager.generateIntData(size, "sorted");
}

std::vector<int> DatasetManager::generateReverseSortedData(std::size_t size) {
  DatasetManager manager;
  return manager.generateIntData(size, "reverse_sorted");
}

std::vector<int> DatasetManager::generatePartiallySortedData(
    std::size_t size, double sortedFraction) {
  DatasetManager manager;
  return manager.generateIntData(size, "partially_sorted", sortedFraction);
}

std::vector<int> DatasetManager::generateTestData(std::size_t size) {
  return DatasetManager::generateRandomData(size);
}