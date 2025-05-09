#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

// Read data from a file and return as vector of integers
std::vector<int> readDataFromFile(const std::string& filename);

// Write vector data to a file
void writeDataToFile(const std::vector<int>& data, const std::string& filename);

// Read/write int64_t data
std::vector<int64_t> readInt64DataFromFile(const std::string& filename);
void writeInt64DataToFile(const std::vector<int64_t>& data,
                          const std::string& filename);
void appendInt64DataToFile(const std::vector<int64_t>& data,
                           const std::string& filename);

// Block operations for external sorting
std::vector<int64_t> readBlockFromFile(const std::string& filename,
                                       std::size_t offset,
                                       std::size_t blockSize);
void writeBlockToFile(const std::string& filename,
                      const std::vector<int64_t>& block, std::size_t offset);

// Disk access counters
extern std::size_t disk_read_count;
extern std::size_t disk_write_count;
void resetDiskCounters();
std::size_t getDiskReadCount();
std::size_t getDiskWriteCount();

#endif  // FILE_HANDLER_H