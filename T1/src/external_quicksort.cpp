#include "external_quicksort.hpp"
#include "disk_io.hpp"
#include <bits/stdc++.h>
#include <ctime>

using namespace std;

// Chooses pivots using reservoir sampling
vector<int64_t> choosePivots(const string& filename, int parts) {
    ifstream in(filename, ios::binary);
    if (!in) {
        cerr << "No se pudo abrir el archivo: " << filename << endl;
        return {};
    }

    size_t fileSize = getFileSize<int64_t>(filename);
    if (fileSize == 0 || parts < 2) return {};

    size_t total_blocks = (fileSize * sizeof(int64_t)) / BLOQUE;

    // Seleccionar un bloque aleatorio
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<size_t> block_dist(0, total_blocks - 1);
    size_t block_index = block_dist(gen);

    in.seekg(block_index * BLOQUE, ios::beg);

    vector<int64_t> buffer(B_LIMIT);
    in.read(reinterpret_cast<char*>(buffer.data()), BLOQUE);
    size_t elements_read = in.gcount() / sizeof(int64_t);

    // Elegir índices únicos dentro del bloque
    unordered_set<size_t> indices;
    uniform_int_distribution<size_t> elem_dist(0, elements_read - 1);
    while (indices.size() < static_cast<size_t>(parts - 1)) {
        indices.insert(elem_dist(gen));
    }

    vector<int64_t> pivots;
    for (size_t idx : indices) {
        pivots.push_back(buffer[idx]);
    }

    sort(pivots.begin(), pivots.end());
    return pivots;
}

// Partitions file into parts+1 temporary files based on pivots
void partitionFile(const string& file, const vector<int64_t>& pivots,
                   vector<string>& outFiles, size_t memBytes) {
    int p = pivots.size() + 1;
    outFiles.clear();
    for (int i = 0; i < p; ++i) {
        string name = file + "_part" + to_string(i);
        ofstream(name, ios::binary).close();
        outFiles.push_back(name);
    }

    size_t bufInts = max<size_t>(1, memBytes / sizeof(int64_t));
    vector<int64_t> buf(bufInts);
    ifstream in(file, ios::binary);
    while (in) {
        in.read(reinterpret_cast<char*>(buf.data()), buf.size() * sizeof(int64_t));
        size_t got = in.gcount() / sizeof(int64_t);
        buf.resize(got);
        for (auto v : buf) {
            int idx = lower_bound(pivots.begin(), pivots.end(), v) - pivots.begin();
            appendInts(outFiles[idx], {v});
        }
    }
}

// External quicksort main function
void externalQuicksort(const string& inFile, const string& outFile,
                       size_t memBytes, int parts) {
    if (getFileSize<int64_t>(inFile) <= memBytes) { // Fixed template arg
        sortInMemory(inFile, outFile);
        return;
    }

    auto pivots = choosePivots(inFile, memBytes, parts);
    vector<string> partsF;
    partitionFile(inFile, pivots, partsF, memBytes);

    ofstream(outFile, ios::binary).close();
    for (auto& f : partsF) {
        string sorted = f + "_sorted";
        externalQuicksort(f, sorted, memBytes, parts);
        auto data = readInts(sorted, 0, getFileSize<int64_t>(sorted) / sizeof(int64_t)); // Fixed
        appendInts(outFile, data);
        remove(f.c_str());
        remove(sorted.c_str());
    }
}