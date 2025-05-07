#include "external_quicksort.hpp"
#include "disk_io.hpp"
#include <bits/stdc++.h>
#include <ctime>

using namespace std;

// Chooses pivots using reservoir sampling
vector<int64_t> choosePivots(const string& filename, size_t memBytes, int parts) {
    size_t total = getFileSize<int64_t>(filename) / sizeof(int64_t); // Fixed template arg
    size_t samp = memBytes / sizeof(int64_t);
    if (samp > total) samp = total;

    vector<int64_t> res;
    res.reserve(samp);
    ifstream in(filename, ios::binary);
    int64_t x;
    size_t seen = 0;
    while (in.read(reinterpret_cast<char*>(&x), sizeof(x))) {
        ++seen;
        if (res.size() < samp) res.push_back(x);
        else if (rand() % seen < samp) res[rand() % samp] = x; // Fixed sign compare
    }

    sort(res.begin(), res.end());
    vector<int64_t> pivots;
    for (int i = 1; i < parts; ++i)
        pivots.push_back(res[i * res.size() / parts]);
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