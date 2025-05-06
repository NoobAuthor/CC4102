#include <iostream>
#include <fstream>
#include <vector>
#include <cassert>
#include <algorithm>
#include "../src/external_mergesort.hpp"

// Helper: write a vector of int64_t to a binary file
void writeBinary(const std::string& filename, const std::vector<int64_t>& data) {
    std::ofstream out(filename, std::ios::binary);
    out.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(int64_t));
}

// Helper: read entire binary file into a vector<int64_t>
std::vector<int64_t> readBinary(const std::string& filename) {
    std::ifstream in(filename, std::ios::binary | std::ios::ate);
    std::streamsize size = in.tellg();
    in.seekg(0, std::ios::beg);
    std::vector<int64_t> data(size / sizeof(int64_t));
    in.read(reinterpret_cast<char*>(data.data()), size);
    return data;
}

int main() {
    // Test data
    std::vector<int64_t> v = {9,1,8,2,7,3,6,4,5,0};
    const std::string inputFile  = "test/merge_input.bin";
    const std::string outputFile = "test/merge_output.bin";

    writeBinary(inputFile, v);

    // Run externalMergesort with memory limit = 3 ints, arity = 3
    externalMergesort(inputFile, outputFile, 3 * sizeof(int64_t), 3);

    // Verify sorting
    auto sorted = readBinary(outputFile);
    auto expect = v;
    std::sort(expect.begin(), expect.end());
    assert(sorted == expect && "externalMergesort failed to sort!");

    std::cout << "[OK] externalMergesort sorted correctly.\n";
    return 0;
}
