#include "utils/file_handler.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

// Global counters for disk access operations
std::size_t disk_read_count = 0;
std::size_t disk_write_count = 0;

void resetDiskCounters() {
  disk_read_count = 0;
  disk_write_count = 0;
}

std::size_t getDiskReadCount() { return disk_read_count; }

std::size_t getDiskWriteCount() { return disk_write_count; }

void appendInt64DataToFile(const std::vector<int64_t>& data,
                           const std::string& filename) {
  // Open file in append mode
  std::ofstream file(filename, std::ios::binary | std::ios::app);

  if (!file.is_open()) {
    // If file doesn't exist, create the directory
    std::filesystem::path path(filename);
    std::filesystem::create_directories(path.parent_path());

    // Try opening again
    file.open(filename, std::ios::binary | std::ios::app);
    if (!file.is_open()) {
      throw std::runtime_error("Could not open file for appending: " +
                               filename);
    }
  }

  // Write data
  if (!file.write(reinterpret_cast<const char*>(data.data()),
                  data.size() * sizeof(int64_t))) {
    throw std::runtime_error("Error writing to file: " + filename);
  }

  // Increment disk write counter
  disk_write_count++;
}

std::vector<int> readDataFromFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + filename);
  }

  // Get file size
  file.seekg(0, std::ios::end);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  // Calculate number of integers
  std::size_t numIntegers = size / sizeof(int);
  std::vector<int> data(numIntegers);

  // Read data
  if (!file.read(reinterpret_cast<char*>(data.data()), size)) {
    throw std::runtime_error("Error reading from file: " + filename);
  }

  // Increment disk read counter
  disk_read_count++;

  return data;
}

void writeDataToFile(const std::vector<int>& data,
                     const std::string& filename) {
  // Make sure directory exists
  std::filesystem::path path(filename);
  std::filesystem::create_directories(path.parent_path());

  std::ofstream file(filename, std::ios::binary | std::ios::trunc);

  if (!file.is_open()) {
    throw std::runtime_error("Could not open file for writing: " + filename);
  }

  // Write data
  if (!file.write(reinterpret_cast<const char*>(data.data()),
                  data.size() * sizeof(int))) {
    throw std::runtime_error("Error writing to file: " + filename);
  }

  // Increment disk write counter
  disk_write_count++;
}

std::vector<int64_t> readInt64DataFromFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + filename);
  }

  // Get file size
  file.seekg(0, std::ios::end);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  // Calculate number of integers
  std::size_t numIntegers = size / sizeof(int64_t);
  std::vector<int64_t> data(numIntegers);

  // Read data
  if (!file.read(reinterpret_cast<char*>(data.data()), size)) {
    throw std::runtime_error("Error reading from file: " + filename);
  }

  // Increment disk read counter
  disk_read_count++;

  return data;
}

void writeInt64DataToFile(const std::vector<int64_t>& data,
                          const std::string& filename) {
  // Make sure directory exists
  std::filesystem::path path(filename);
  std::filesystem::create_directories(path.parent_path());

  std::ofstream file(filename, std::ios::binary | std::ios::trunc);

  if (!file.is_open()) {
    throw std::runtime_error("Could not open file for writing: " + filename);
  }

  // Write data
  if (!file.write(reinterpret_cast<const char*>(data.data()),
                  data.size() * sizeof(int64_t))) {
    throw std::runtime_error("Error writing to file: " + filename);
  }

  // Increment disk write counter
  disk_write_count++;
}

std::vector<int64_t> readBlockFromFile(const std::string& filename,
                                       std::size_t offset,
                                       std::size_t blockSize) {
  std::ifstream file(filename, std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + filename);
  }

  // Seek to the specified offset
  file.seekg(offset, std::ios::beg);

  // Calculate how many int64_t values we can read
  std::size_t maxElements = blockSize / sizeof(int64_t);
  std::vector<int64_t> block(maxElements);

  // Read the block
  file.read(reinterpret_cast<char*>(block.data()), blockSize);

  // Get how many elements were actually read
  std::size_t elementsRead = file.gcount() / sizeof(int64_t);
  block.resize(elementsRead);

  // Increment disk read counter
  disk_read_count++;

  return block;
}

void writeBlockToFile(const std::string& filename,
                      const std::vector<int64_t>& block, std::size_t offset) {
  // Make sure directory exists
  std::filesystem::path path(filename);
  std::filesystem::create_directories(path.parent_path());

  std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);

  if (!file.is_open()) {
    // If file doesn't exist, create it
    std::ofstream newFile(filename, std::ios::binary);
    newFile.close();

    file.open(filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
      throw std::runtime_error("Could not create file: " + filename);
    }
  }

  // Seek to the specified offset
  file.seekp(offset, std::ios::beg);

  // Write the block
  file.write(reinterpret_cast<const char*>(block.data()),
             block.size() * sizeof(int64_t));

  if (!file) {
    throw std::runtime_error("Error writing to file: " + filename);
  }

  // Increment disk write counter
  disk_write_count++;
}