#ifndef EXTERNAL_QUICKSORT_HPP
#define EXTERNAL_QUICKSORT_HPP

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

// Returns file size in bytes
size_t getFileSize(const std::string& filename);

// Reads up to 'count' int64_t values starting from 'start' index
std::vector<int64_t> readInts(const std::string& filename, size_t start, size_t count);

// Appends values to a binary file
void appendInts(const std::string& filename, const std::vector<int64_t>& data);

// Sorts small files in memory
void sortInMemory(const std::string& inFile, const std::string& outFile);

// Chooses pivots using reservoir sampling
std::vector<int64_t> choosePivots(const std::string& filename, size_t memBytes, int parts);

// Partitions file into 'parts+1' temporary files based on pivots
void partitionFile(const std::string& file, const std::vector<int64_t>& pivots,
                   std::vector<std::string>& outFiles, size_t memBytes);

// External quicksort main function
void externalQuicksort(const std::string& inFile, const std::string& outFile,
                       size_t memBytes, int parts);

#endif // EXTERNAL_QUICKSORT_HPP
