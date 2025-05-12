#include "utils/file_handler.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

size_t disk_read_count = 0;
size_t disk_write_count = 0;

void resetDiskCounters() {
  disk_read_count = 0;
  disk_write_count = 0;
}

size_t getDiskReadCount() { return disk_read_count; }

size_t getDiskWriteCount() { return disk_write_count; }

void ensureDirectoryExists(const std::string& filepath) {
  size_t lastSlash = filepath.find_last_of("/\\");
  if (lastSlash != std::string::npos) {
    std::string directory = filepath.substr(0, lastSlash);
    std::filesystem::create_directories(directory);
  }
}

std::vector<int> readFromFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + filename);
  }

  file.seekg(0, std::ios::end);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  size_t numIntegers = size / sizeof(int);
  std::vector<int> data(numIntegers);

  if (!file.read(reinterpret_cast<char*>(data.data()), size)) {
    throw std::runtime_error("Error reading from file: " + filename);
  }

  disk_read_count++;

  return data;
}

std::vector<int> readDataFromFile(const std::string& filename) {
  return readFromFile(filename);
}

void writeToFile(const std::string& filename, const std::vector<int>& data) {
  ensureDirectoryExists(filename);

  std::ofstream file(filename, std::ios::binary | std::ios::trunc);

  if (!file.is_open()) {
    throw std::runtime_error("Could not open file for writing: " + filename);
  }

  if (!file.write(reinterpret_cast<const char*>(data.data()),
                  data.size() * sizeof(int))) {
    throw std::runtime_error("Error writing to file: " + filename);
  }

  disk_write_count++;
}

void writeDataToFile(const std::string& filename,
                     const std::vector<int>& data) {
  writeToFile(filename, data);
}

std::vector<int64_t> readInt64FromFile(const std::string& filename) {
  std::ifstream file(filename, std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + filename);
  }

  file.seekg(0, std::ios::end);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  size_t numIntegers = size / sizeof(int64_t);
  std::vector<int64_t> data(numIntegers);

  if (!file.read(reinterpret_cast<char*>(data.data()), size)) {
    throw std::runtime_error("Error reading from file: " + filename);
  }

  disk_read_count++;

  return data;
}

std::vector<int64_t> readInt64DataFromFile(const std::string& filename) {
  return readInt64FromFile(filename);
}

void writeInt64ToFile(const std::string& filename,
                      const std::vector<int64_t>& data) {
  ensureDirectoryExists(filename);

  std::ofstream file(filename, std::ios::binary | std::ios::trunc);

  if (!file.is_open()) {
    throw std::runtime_error("Could not open file for writing: " + filename);
  }

  if (!file.write(reinterpret_cast<const char*>(data.data()),
                  data.size() * sizeof(int64_t))) {
    throw std::runtime_error("Error writing to file: " + filename);
  }

  disk_write_count++;
}

void writeInt64DataToFile(const std::vector<int64_t>& data,
                          const std::string& filename) {
  writeInt64ToFile(filename, data);
}

std::vector<int64_t> readBlockFromFile(const std::string& filename,
                                       size_t offset, size_t blockSize) {
  std::ifstream file(filename, std::ios::binary);

  if (!file.is_open()) {
    throw std::runtime_error("Could not open file: " + filename);
  }

  file.seekg(offset, std::ios::beg);

  size_t maxElements = blockSize / sizeof(int64_t);
  std::vector<int64_t> block(maxElements);

  file.read(reinterpret_cast<char*>(block.data()), blockSize);

  size_t elementsRead = file.gcount() / sizeof(int64_t);
  block.resize(elementsRead);

  disk_read_count++;

  return block;
}

void writeBlockToFile(const std::string& filename,
                      const std::vector<int64_t>& block, size_t offset) {
  ensureDirectoryExists(filename);

  std::fstream file(filename, std::ios::binary | std::ios::in | std::ios::out);

  if (!file.is_open()) {
    std::ofstream newFile(filename, std::ios::binary);
    newFile.close();

    file.open(filename, std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
      throw std::runtime_error("Could not create file: " + filename);
    }
  }

  file.seekp(offset, std::ios::beg);

  file.write(reinterpret_cast<const char*>(block.data()),
             block.size() * sizeof(int64_t));

  if (!file) {
    throw std::runtime_error("Error writing to file: " + filename);
  }

  disk_write_count++;
}

void appendInt64DataToFile(const std::vector<int64_t>& data,
                           const std::string& filename) {
  std::ofstream file(filename, std::ios::binary | std::ios::app);

  if (!file.is_open()) {
    throw std::runtime_error("Could not open file for appending: " + filename);
  }

  if (!file.write(reinterpret_cast<const char*>(data.data()),
                  data.size() * sizeof(int64_t))) {
    throw std::runtime_error("Error appending to file: " + filename);
  }

  disk_write_count++;
}