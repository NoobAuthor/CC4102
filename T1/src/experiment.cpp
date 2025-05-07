#include <cstddef>
#include "external_quicksort.hpp"
#include "external_mergesort.hpp"
#include <chrono>
#include <random>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <numeric>
#include <climits>

 // Global disk access counters and block size
uint64_t readCount = 0, writeCount = 0;
size_t BLOCK_SIZE = 0;
// Function to set the block size
void setBlockSize(size_t blockSize) {
    BLOCK_SIZE = blockSize;
}
// Function to write binary data to a file
void writeBinary(const std::string& filename, const std::vector<int64_t>& data) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        throw std::ios_base::failure("Failed to open file for writing: " + filename);
    }
    out.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(int64_t));
    out.close();
}

// Wrapper functions with I/O accounting
std::vector<int64_t> readCounted(const std::string& f, size_t s, size_t c) {
    auto v = readInts(f, s, c);
    size_t bytes = v.size() * sizeof(int64_t);
    readCount += (bytes + BLOCK_SIZE - 1) / BLOCK_SIZE;
    return v;
}

void appendCounted(const std::string& f, const std::vector<int64_t>& d) {
    appendInts(f, d);
    size_t bytes = d.size() * sizeof(int64_t);
    writeCount += (bytes + BLOCK_SIZE - 1) / BLOCK_SIZE;
}

// Binary search for optimal merge arity
int findOptimalArity(const std::string& filename, size_t B, size_t M) {
    size_t max_arity = B / sizeof(int64_t);
    int low = 2, high = max_arity, best_arity = 2;
    uint64_t min_io = UINT64_MAX;

    // Generate 60M dataset if not exists
    std::ifstream test(filename);
    if (!test.good()) {
        std::mt19937_64 rng(123);
        std::vector<int64_t> data(60 * 1024 * 1024 / sizeof(int64_t));
        std::iota(data.begin(), data.end(), 0);
        std::shuffle(data.begin(), data.end(), rng);
        writeBinary(filename, data);
    }

    for (int arity = low; arity <= high; ++arity) {
        setBlockSize(B);
        readCount = writeCount = 0;
        
        try {
            externalMergesort(filename, "temp.bin", M, arity);
            uint64_t total_io = readCount + writeCount;
            
            if (total_io < min_io) {
                min_io = total_io;
                best_arity = arity;
            }
        } catch (...) {
            std::cerr << "Error testing arity=" << arity << "\n";
        }
        remove("temp.bin");
    }
    return best_arity;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " B_bytes M_bytes\n";
        return 1;
    }

    const size_t B = std::stoull(argv[1]);
    const size_t M = std::stoull(argv[2]);
    const int runs = 5;
    const std::vector<size_t> multipliers = {4,8,12,16,20,24,28,32,36,40,44,48,52,56,60};
    
    // Determine optimal arity using 60M dataset
    const std::string largeFile = "60M_input.bin";
    const int best_arity = findOptimalArity(largeFile, B, M);
    std::cout << "Optimal arity: " << best_arity << "\n";

    // Experiment setup
    std::mt19937_64 rng(123);
    std::ofstream res("results.csv");
    res << "N,algorithm,avg_time_ms,avg_reads,avg_writes\n";

    for (auto k : multipliers) {
        const size_t N = k * M / sizeof(int64_t);
        double sum_time_q = 0, sum_time_m = 0;
        uint64_t sum_rq = 0, sum_wq = 0, sum_rm = 0, sum_wm = 0;

        for (int i = 0; i < runs; ++i) {
            // Generate dataset
            std::vector<int64_t> data(N);
            std::iota(data.begin(), data.end(), 0);
            std::shuffle(data.begin(), data.end(), rng);
            const std::string inFile = "input.bin";
            writeBinary(inFile, data);

            // Quicksort run
            setBlockSize(B);
            auto t0 = std::chrono::high_resolution_clock::now();
            externalQuicksort(inFile, "outQ.bin", M, best_arity);
            auto t1 = std::chrono::high_resolution_clock::now();
            sum_time_q += std::chrono::duration<double, std::milli>(t1-t0).count();
            sum_rq += readCount; sum_wq += writeCount;

            // Mergesort run
            setBlockSize(B);
            t0 = std::chrono::high_resolution_clock::now();
            externalMergesort(inFile, "outM.bin", M, best_arity);
            t1 = std::chrono::high_resolution_clock::now();
            sum_time_m += std::chrono::duration<double, std::milli>(t1-t0).count();
            sum_rm += readCount; sum_wm += writeCount;

            // Cleanup
            remove(inFile.c_str());
            remove("outQ.bin");
            remove("outM.bin");
        }

        // Write averaged results
        res << N << ",QUICKSORT," << (sum_time_q/runs) << "," 
            << (sum_rq/runs) << "," << (sum_wq/runs) << "\n";
        res << N << ",MERGESORT," << (sum_time_m/runs) << "," 
            << (sum_rm/runs) << "," << (sum_wm/runs) << "\n";
    }

    remove(largeFile.c_str());
    return 0;
}