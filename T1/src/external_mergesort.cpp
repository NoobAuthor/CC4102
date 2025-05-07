#include "disk_io.hpp"
#include "external_mergesort.hpp"
#include <bits/stdc++.h>
#include <queue>

using namespace std;

vector<string> createInitialRuns(const string& inFile, size_t memBytes) {
    size_t intsPerRun = max<size_t>(1, memBytes / sizeof(int64_t));
    ifstream in(inFile, ios::binary);
    vector<int64_t> buf(intsPerRun);
    vector<string> runs;
    int idx = 0;
    while (in) {
        in.read(reinterpret_cast<char*>(buf.data()), buf.size() * sizeof(int64_t));
        size_t got = in.gcount() / sizeof(int64_t);
        if (got == 0) break;
        buf.resize(got);
        sort(buf.begin(), buf.end());
        string runName = inFile + "_run" + to_string(idx++);
        ofstream out(runName, ios::binary);
        out.write(reinterpret_cast<char*>(buf.data()), got * sizeof(int64_t));
        runs.push_back(runName);
        buf.resize(intsPerRun);
    }
    return runs;
}

void mergeRuns(vector<string>& runFiles, size_t /*memBytes*/, int arity) {
    int pass = 0;
    while (runFiles.size() > 1) {
        vector<string> next;
        for (size_t i = 0; i < runFiles.size(); i += arity) {
            size_t end = min(i + arity, runFiles.size());
            vector<ifstream> ins;
            for (size_t j = i; j < end; ++j)
                ins.emplace_back(runFiles[j], ios::binary);
            string outName = runFiles[i] + "_m" + to_string(pass);
            ofstream out(outName, ios::binary);
            using P = pair<int64_t,int>;
            auto cmp = [](const P &a, const P &b){ return a.first > b.first; };
            priority_queue<P, vector<P>, decltype(cmp)> pq(cmp);
            // initial load
            for (size_t k = 0; k < ins.size(); ++k) {
                int64_t x;
                if (ins[k].read(reinterpret_cast<char*>(&x), sizeof(x)))
                    pq.emplace(x, k);
            }
            // k-way merge
            while (!pq.empty()) {
                auto [val, idx] = pq.top(); pq.pop();
                out.write(reinterpret_cast<char*>(&val), sizeof(val));
                int64_t x;
                if (ins[idx].read(reinterpret_cast<char*>(&x), sizeof(x)))
                    pq.emplace(x, idx);
            }
            // cleanup
            for (size_t j = i; j < end; ++j)
                remove(runFiles[j].c_str());
            next.push_back(outName);
        }
        runFiles.swap(next);
        ++pass;
    }
}

void externalMergesort(const string& inFile,
                       const string& outFile,
                       size_t memBytes,
                       int arity) {
    if (::getFileSize<int64_t>(inFile) <= memBytes) {
        sortInMemory(inFile, outFile);
        return;
    }
    auto runs = createInitialRuns(inFile, memBytes);
    mergeRuns(runs, memBytes, arity);
    if (!runs.empty()) {
        rename(runs[0].c_str(), outFile.c_str());
    } else {
        ofstream(outFile, ios::binary).close();
    }
}
