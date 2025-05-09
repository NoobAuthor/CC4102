#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "utils/file_handler.h"
#include "utils/sort_parameters.h"
#include "utils/test_generator.h"

// Generate sequence of specified size and save to binary file
void generateDataSequence(size_t size, const std::string& filename) {
  std::cout << "Generating data sequence of size " << size << " ("
            << (size * INT64_SIZE / MB) << "MB)" << std::endl;

  // Generate random 64-bit integers
  std::vector<int64_t> data;
  data.reserve(size);

  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<int64_t> distrib(INT64_MIN, INT64_MAX);

  for (size_t i = 0; i < size; i++) {
    data.push_back(distrib(gen));
  }

  // Write to binary file
  writeInt64DataToFile(data, filename);
  std::cout << "Wrote " << data.size() << " elements to " << filename
            << std::endl;
}

int main() {
  // Create directories if they don't exist
  std::filesystem::create_directories("data/sequences");

  const size_t M_ELEMENTS =
      mbToElementCount(50);  // Number of elements in M (50MB)

  // Generate 5 sequences for each size N ∈ {4M, 8M, ..., 60M}
  for (size_t multiplier = 4; multiplier <= 60; multiplier += 4) {
    size_t N = multiplier * M_ELEMENTS;

    std::cout << "Generating 5 sequences of size " << multiplier << "M (" << N
              << " elements)" << std::endl;

    for (int sequence = 1; sequence <= 5; sequence++) {
      std::string filename = "data/sequences/seq_" +
                             std::to_string(multiplier) + "M_" +
                             std::to_string(sequence) + ".bin";
      generateDataSequence(N, filename);
    }
  }

  // Also generate a single large sequence for optimal arity determination
  std::string arityTestFile = "data/sequences/arity_test_60M.bin";
  generateDataSequence(60 * M_ELEMENTS, arityTestFile);

  std::cout << "All sequences generated successfully!" << std::endl;
  return 0;
}