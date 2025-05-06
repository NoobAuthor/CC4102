#ifndef EXTERNAL_MERGESORT_HPP
#define EXTERNAL_MERGESORT_HPP

#include <cstdint>
#include <cstddef>
#include <vector>
#include <string>

// Returns file size in bytes
template<typename T>
size_t getFileSize(const std::string& filename);

// Reads up to 'count' int64_t values starting from 'start' index
std::vector<int64_t> readInts(const std::string& filename, size_t start, size_t count);

// Appends values to a binary file
void appendInts(const std::string& filename, const std::vector<int64_t>& data);

// Sorts a small file entirely in memory
void sortInMemory(const std::string& inFile, const std::string& outFile);

// Creates sorted runs of size <= memBytes and returns their filenames
std::vector<std::string> createInitialRuns(const std::string& inFile, size_t memBytes);

// Merges runs in multiple passes using up to 'arity' runs per merge
void mergeRuns(std::vector<std::string>& runFiles, size_t memBytes, int arity);

// The main external mergesort function
void externalMergesort(const std::string& inFile,
                       const std::string& outFile,
                       size_t memBytes,
                       int arity);

#endif // EXTERNAL_MERGESORT_HPP