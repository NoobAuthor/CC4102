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
#include <cstddef>

// Function to write a vector of integers to a binary file
void writeBinary(const std::string& filename, const std::vector<int64_t>& data) {
    std::ofstream out(filename, std::ios::binary);
    if (!out) {
        throw std::ios_base::failure("Failed to open file for writing: " + filename);
    }
    out.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(int64_t));
    out.close();
}

using namespace std;

// Contadores globales de acceso a disco
uint64_t readCount = 0, writeCount = 0;
size_t BLOCK_SIZE = 0;

// Configuración del tamaño de bloque
void setBlockSize(size_t blockSize) { BLOCK_SIZE = blockSize; }
size_t getReads() { return readCount; }
size_t getWrites() { return writeCount; }

// Wrappers para contar operaciones de I/O
vector<int64_t> readCounted(const string& f, size_t s, size_t c) {
    auto v = readInts(f, s, c);
    size_t bytes = v.size() * sizeof(int64_t);
    readCount += (bytes + BLOCK_SIZE - 1) / BLOCK_SIZE;
    return v;
}

void appendCounted(const string& f, const vector<int64_t>& d) {
    appendInts(f, d);
    size_t bytes = d.size() * sizeof(int64_t);
    writeCount += (bytes + BLOCK_SIZE - 1) / BLOCK_SIZE;
}

// Búsqueda binaria de aridad óptima según 4.3.2
int findOptimalArity(const string& filename, size_t B, size_t M) {
    const size_t b = B / sizeof(int64_t); // Números por bloque
    int low = 2, high = b, best_arity = 2;
    uint64_t min_io = UINT64_MAX;
    
    // Generar dataset de 60M si no existe
    ifstream test(filename);
    if (!test.good()) {
        vector<int64_t> data(60 * 1024 * 1024 / sizeof(int64_t));
        iota(data.begin(), data.end(), 0);
        shuffle(data.begin(), data.end(), mt19937_64(123));
        writeBinary(filename, data);
    }

    // Búsqueda binaria
    while (low <= high) {
        int mid = (low + high) / 2;
        
        // Medir IOs para aridad actual
        setBlockSize(B);
        readCount = writeCount = 0;
        try {
            externalMergesort(filename, "temp.bin", M, mid);
            uint64_t total_io = readCount + writeCount;
            
            // Actualizar mejor aridad
            if (total_io < min_io) {
                min_io = total_io;
                best_arity = mid;
            }
            
            // Reducir espacio de búsqueda
            if (mid > best_arity) high = mid - 1;
            else low = mid + 1;
        } catch (...) {
            high = mid - 1; // Reducir aridad si falla
        }
        remove("temp.bin");
    }
    return best_arity;
}

int main(int argc, char** argv) {
    if (argc != 3) {
        cerr << "Uso: " << argv[0] << " B_bytes M_bytes\n";
        return 1;
    }

    const size_t B = stoull(argv[1]);
    const size_t M = stoull(argv[2]);
    const int runs = 5;
    const vector<size_t> multipliers = {4,8,12,16,20,24,28,32,36,40,44,48,52,56,60};

    // Paso 1: Determinar aridad óptima con dataset de 60M
    const string largeFile = "60M_input.bin";
    const int best_arity = findOptimalArity(largeFile, B, M);
    cout << "Aridad óptima encontrada: " << best_arity << "\n";

    // Paso 2: Experimentación con aridad óptima
    ofstream res("results.csv");
    res << "N,alg,avg_time_ms,avg_reads,avg_writes\n";
    
    mt19937_64 rng(123);
    for (auto k : multipliers) {
        const size_t N = k * M / sizeof(int64_t);
        double sumTimeQ = 0, sumTimeM = 0;
        uint64_t sumRQ = 0, sumWQ = 0, sumRM = 0, sumWM = 0;

        for (int i = 0; i < runs; ++i) {
            // Generar datos
            vector<int64_t> data(N);
            iota(data.begin(), data.end(), 0);
            shuffle(data.begin(), data.end(), rng);
            const string inFile = "input.bin";
            writeBinary(inFile, data);

            // Quicksort
            setBlockSize(B);
            auto t0 = chrono::high_resolution_clock::now();
            externalQuicksort(inFile, "outQ.bin", M, best_arity);
            auto t1 = chrono::high_resolution_clock::now();
            sumTimeQ += chrono::duration<double, milli>(t1-t0).count();
            sumRQ += readCount; sumWQ += writeCount;

            // Mergesort
            setBlockSize(B);
            t0 = chrono::high_resolution_clock::now();
            externalMergesort(inFile, "outM.bin", M, best_arity);
            t1 = chrono::high_resolution_clock::now();
            sumTimeM += chrono::duration<double, milli>(t1-t0).count();
            sumRM += readCount; sumWM += writeCount;

            // Limpieza
            remove(inFile.c_str());
            remove("outQ.bin");
            remove("outM.bin");
        }

        // Registrar resultados
        res << N << ",QUICK," << (sumTimeQ/runs) << "," 
           << (sumRQ/runs) << "," << (sumWQ/runs) << "\n";
        res << N << ",MERGE," << (sumTimeM/runs) << "," 
           << (sumRM/runs) << "," << (sumWM/runs) << "\n";
    }

    remove(largeFile.c_str());
    return 0;
}