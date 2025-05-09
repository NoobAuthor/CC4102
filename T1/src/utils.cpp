#include "utils.hpp"

#include <iostream>
#include <limits>
#include <random>
#include <vector>

// Constants
const size_t MB = 1024 * 1024;  // 1 MB in bytes
const size_t M = 50 * MB;       // M = 50 MB (max memory for sorting)
const size_t ELEMENT_SIZE = sizeof(int64_t);  // Size of a 64-bit integer

bool read_block(std::ifstream& file, void* buffer, size_t block_size,
                size_t offset) {
  file.seekg(offset * block_size);
  if (!file) return false;  // Added return

  file.read(static_cast<char*>(buffer), block_size);
  disk_reads++;
  return file.gcount() > 0;
}

bool write_block(std::ofstream& file, const void* buffer, size_t block_size,
                 size_t offset) {
  file.seekp(offset * block_size);
  if (!file) return false;  // Added return

  file.write(static_cast<const char*>(buffer), block_size);
  disk_writes++;
  return file.good();
}

bool generate_random_array(const std::string& filename, size_t n,
                           size_t block_size) {
  std::ofstream file(filename, std::ios::binary);
  if (!file) {
    std::cerr << "Failed to create file: " << filename << std::endl;
    return false;
  }

  // Calculate how many elements fit in one block
  size_t elements_per_block = block_size / ELEMENT_SIZE;

  // Create a buffer for one block
  std::vector<int64_t> buffer(elements_per_block);

  // Use a good random number generator
  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<int64_t> dist(
      std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max());

  // Generate and write blocks
  size_t blocks_needed = (n + elements_per_block - 1) / elements_per_block;

  for (size_t block = 0; block < blocks_needed; ++block) {
    // Generate random numbers for this block
    size_t elements_in_this_block =
        (block == blocks_needed - 1 && n % elements_per_block != 0)
            ? n % elements_per_block
            : elements_per_block;

    for (size_t i = 0; i < elements_in_this_block; ++i) {
      buffer[i] = dist(gen);
    }

    // Write the block
    file.write(reinterpret_cast<char*>(buffer.data()),
               elements_in_this_block * ELEMENT_SIZE);

    if (!file) {
      std::cerr << "Error writing to file" << std::endl;
      return false;
    }
  }

  file.close();
  return true;
}

bool check_sorted(const std::string& filename, size_t n, size_t block_size) {
  std::ifstream file(filename, std::ios::binary);
  if (!file) {
    std::cerr << "Failed to open file: " << filename << std::endl;
    return false;
  }

  // Calculate how many elements fit in one block
  size_t elements_per_block = block_size / ELEMENT_SIZE;

  // Create buffers for reading
  std::vector<int64_t> buffer(elements_per_block);
  int64_t prev = std::numeric_limits<int64_t>::min();

  // Read and check blocks
  size_t elements_read = 0;

  while (elements_read < n) {
    size_t elements_to_read = std::min(elements_per_block, n - elements_read);

    file.read(reinterpret_cast<char*>(buffer.data()),
              elements_to_read * ELEMENT_SIZE);
    size_t elements_actually_read = file.gcount() / ELEMENT_SIZE;

    if (elements_actually_read == 0) {
      break;
    }

    for (size_t i = 0; i < elements_actually_read; ++i) {
      if (buffer[i] < prev) {
        return false;
      }
      prev = buffer[i];
    }

    elements_read += elements_actually_read;
  }

  file.close();
  return true;
}