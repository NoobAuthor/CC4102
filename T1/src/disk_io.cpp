#include "disk_io.hpp"
#include <fstream>     // For file I/O
#include <string>      // Explicit include for std::string
#include <algorithm>   // For std::sort

// Template implementation with explicit instantiation
template<typename T>
size_t getFileSize(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary | std::ios::ate);
    return in.tellg();
}

// Explicit instantiation for int64_t
template size_t getFileSize<int64_t>(const std::string&);

// Read integers from a file
std::vector<int64_t> readInts(const std::string& filename, size_t start, size_t count) {
    std::vector<int64_t> data(count);
    std::ifstream in(filename, std::ios::binary);
    in.seekg(start * sizeof(int64_t), std::ios::beg);
    in.read(reinterpret_cast<char*>(data.data()), count * sizeof(int64_t));
    data.resize(in.gcount() / sizeof(int64_t));
    return data;
}

// Append integers to a file
void appendInts(const std::string& filename, const std::vector<int64_t>& data) {
    std::ofstream out(filename, std::ios::binary | std::ios::app);
    out.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(int64_t));
}

// Sort a small file in memory
void sortInMemory(const std::string& inFile, const std::string& outFile) {
    size_t n = getFileSize<int64_t>(inFile) / sizeof(int64_t);
    auto v = readInts(inFile, 0, n);
    std::sort(v.begin(), v.end());
    std::ofstream out(outFile, std::ios::binary);
    out.write(reinterpret_cast<const char*>(v.data()), v.size() * sizeof(int64_t));
}