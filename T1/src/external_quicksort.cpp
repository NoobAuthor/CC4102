#include "external_quicksort.hpp"
#include <bits/stdc++.h>
#include <ctime>

using namespace std;

// geFileSize returns the size of a file in bytes
size_t getFileSize(const string& filename) {
    ifstream in(filename, ios::binary | ios::ate);
    return in.tellg();
}

// readInts reads 'count' int64_t values from a file starting at 'start' index
vector<int64_t> readInts(const string& filename, size_t start, size_t count) {
    vector<int64_t> data(count);
    ifstream in(filename, ios::binary);
    in.seekg(start * sizeof(int64_t), ios::beg);
    in.read(reinterpret_cast<char*>(data.data()), count * sizeof(int64_t));
    data.resize(in.gcount() / sizeof(int64_t));
    return data;
}

// appendInts appends int64_t values to a binary file
void appendInts(const string& filename, const vector<int64_t>& data) {
    ofstream out(filename, ios::binary | ios::app);
    out.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(int64_t));
}

// sortInMemory sorts a small file in memory and writes the sorted data to another file
void sortInMemory(const string& inFile, const string& outFile) {
    size_t n = getFileSize(inFile) / sizeof(int64_t);
    auto v = readInts(inFile, 0, n);
    sort(v.begin(), v.end());
    ofstream out(outFile, ios::binary);
    out.write(reinterpret_cast<const char*>(v.data()), v.size() * sizeof(int64_t));
}

// choosePivots selects 'parts' pivots from the file using reservoir sampling
vector<int64_t> choosePivots(const string& filename, size_t memBytes, int parts) {
    size_t total = getFileSize(filename) / sizeof(int64_t);
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
        else if (rand() % seen < int(samp)) res[rand() % samp] = x;
    }

    sort(res.begin(), res.end());
    vector<int64_t> pivots;
    for (int i = 1; i < parts; ++i)
        pivots.push_back(res[i * res.size() / parts]);
    return pivots;
}

// partitionFile partitions the input file into 'parts+1' temporary files based on pivots
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

// externalQuicksort performs external quicksort on a large file
void externalQuicksort(const string& inFile, const string& outFile,
                       size_t memBytes, int parts) {
    if (getFileSize(inFile) <= memBytes) {
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
        auto data = readInts(sorted, 0, getFileSize(sorted) / sizeof(int64_t));
        appendInts(outFile, data);
        remove(f.c_str());
        remove(sorted.c_str());
    }
}