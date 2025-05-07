#ifndef DISK_IO_HPP
#define DISK_IO_HPP

#include <string>       // Include for std::string
#include <vector>       // Include for std::vector
#include <cstdint>      // For int64_t
#include <cstddef>      // For size_t

// Template declaration for file size
template<typename T>
size_t getFileSize(const std::string& filename);

// Read 'count' int64_t values starting at 'start'
std::vector<int64_t> readInts(const std::string& filename, size_t start, size_t count);

// Append values to a binary file
void appendInts(const std::string& filename, const std::vector<int64_t>& data);

// Sort small files in memory
void sortInMemory(const std::string& inFile, const std::string& outFile);

#endif