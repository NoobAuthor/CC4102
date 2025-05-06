#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <algorithm>
#include "../src/external_quicksort.hpp"

// Helper: write vector<int64_t> to binary file
void writeBinary(const std::string& filename, const std::vector<int64_t>& data) {
    std::ofstream out(filename, std::ios::binary);
    out.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(int64_t));
}

// Helper: read entire binary file into vector<int64_t>
std::vector<int64_t> readBinary(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary | std::ios::ate);
    std::streamsize size = in.tellg();
    in.seekg(0, std::ios::beg);
    std::vector<int64_t> data(size / sizeof(int64_t));
    in.read(reinterpret_cast<char*>(data.data()), size);
    return data;
}

int main() {
    // Prepare test data
    std::vector<int64_t> data = {5, 3, 8, 1, 2, 7, 4, 6};
    const std::string inputFile  = "test/input.bin";
    const std::string outputFile = "test/output.bin";

    writeBinary(inputFile, data);

    // Run externalQuicksort (memory limit = 4 ints, 2 partitions)
    size_t memLimit = 4 * sizeof(int64_t);
    int partitions  = 2;
    externalQuicksort(inputFile, outputFile, memLimit, partitions);

    // Read and verify
    std::vector<int64_t> sortedData = readBinary(outputFile);
    std::vector<int64_t> expected   = data;
    std::sort(expected.begin(), expected.end());
    assert(sortedData == expected && "Sorted output does not match expected!");

    std::cout << "[OK] externalQuicksort sorted the data correctly.\n";
    return 0;
}
