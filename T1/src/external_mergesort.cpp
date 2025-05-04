#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <filesystem>
#include <cstdio>
#include <cstdint>  // para int64_t

const size_t BLOQUE = 1024;
const size_t B_LIMIT = BLOQUE / sizeof(int64_t);  // Cambié el tamaño del bloque a 64 bits

using namespace std;
namespace fs = std::filesystem;

void refill_buffer(ifstream& in, vector<int64_t>& buffer, size_t& count) {
    in.read(reinterpret_cast<char*>(buffer.data()), BLOQUE);
    count = in.gcount() / sizeof(int64_t);
}

void phase1_split_and_sort(const string& input_file, vector<string>& temp_files) {
    ifstream infile(input_file, ios::binary);
    if (!infile) {
        cerr << "Error al abrir archivo de entrada." << endl;
        return;
    }

    vector<int64_t> buffer(B_LIMIT);
    int part = 0;

    while (!infile.eof()) {
        infile.read(reinterpret_cast<char*>(buffer.data()), BLOQUE);
        size_t items_read = infile.gcount() / sizeof(int64_t);
        if (items_read == 0) break;

        sort(buffer.begin(), buffer.begin() + items_read);

        string temp_name = "temp_" + to_string(part++) + ".bin";
        ofstream temp_out(temp_name, ios::binary);
        if (!temp_out) {
            cerr << "Error al crear archivo temporal." << endl;
            return;
        }
        temp_out.write(reinterpret_cast<char*>(buffer.data()), items_read * sizeof(int64_t));
        if (!temp_out) {
            cerr << "Error al escribir en archivo temporal." << endl;
            return;
        }

        temp_files.push_back(temp_name);
    }
}

void merge_two_files(const string& file1, const string& file2, const string& output_file) {
    ifstream in1(file1, ios::binary);
    ifstream in2(file2, ios::binary);
    ofstream out(output_file, ios::binary);

    if (!in1 || !in2 || !out) {
        cerr << "Error al abrir archivos para fusionar." << endl;
        return;
    }

    vector<int64_t> buffer1(B_LIMIT), buffer2(B_LIMIT), buffer_out(B_LIMIT);
    size_t idx1 = 0, idx2 = 0, out_idx = 0;
    size_t read1 = 0, read2 = 0;

    refill_buffer(in1, buffer1, read1);
    refill_buffer(in2, buffer2, read2);

    while (read1 > 0 || read2 > 0) {
        while (idx1 < read1 && idx2 < read2) {
            if (buffer1[idx1] <= buffer2[idx2]) {
                buffer_out[out_idx++] = buffer1[idx1++];
            } else {
                buffer_out[out_idx++] = buffer2[idx2++];
            }
            if (out_idx == B_LIMIT) {
                out.write(reinterpret_cast<char*>(buffer_out.data()), out_idx * sizeof(int64_t));
                out_idx = 0;
            }
        }

        if (idx1 == read1 && in1) {
            refill_buffer(in1, buffer1, read1);
            idx1 = 0;
        }
        if (idx2 == read2 && in2) {
            refill_buffer(in2, buffer2, read2);
            idx2 = 0;
        }
    }

    while (idx1 < read1) {
        buffer_out[out_idx++] = buffer1[idx1++];
        if (out_idx == B_LIMIT) {
            out.write(reinterpret_cast<char*>(buffer_out.data()), out_idx * sizeof(int64_t));
            out_idx = 0;
        }
    }
    while (idx2 < read2) {
        buffer_out[out_idx++] = buffer2[idx2++];
        if (out_idx == B_LIMIT) {
            out.write(reinterpret_cast<char*>(buffer_out.data()), out_idx * sizeof(int64_t));
            out_idx = 0;
        }
    }

    if (out_idx > 0) {
        out.write(reinterpret_cast<char*>(buffer_out.data()), out_idx * sizeof(int64_t));
    }
}

void phase2_merge(vector<string> temp_files, const string& output_file, int arity = 0) {
    while (temp_files.size() > 1) {
        vector<string> next_round;

        for (size_t i = 0; i < temp_files.size(); i += 2) {
            if (i + 1 == temp_files.size()) {
                next_round.push_back(temp_files[i]);
            } else {
                string merged_file = "merge_" + to_string(arity) + "_" + to_string(i / 2) + ".bin";
                merge_two_files(temp_files[i], temp_files[i + 1], merged_file);

                fs::remove(temp_files[i]);
                fs::remove(temp_files[i + 1]);
                next_round.push_back(merged_file);
            }
        }
        temp_files = move(next_round);
        ++arity;
    }

    if (!temp_files.empty()) {
        if (fs::exists(output_file)) {
            fs::remove(output_file);
        }
        fs::rename(temp_files[0], output_file);
    }
}

void sort_large_binary_file(const string& input_file, const string& output_file) {
    vector<string> temp_files;
    phase1_split_and_sort(input_file, temp_files);
    if (!temp_files.empty()) {
        phase2_merge(temp_files, output_file);
        cout << "Ordenamiento completo." << endl;
    }
}

int main() {
    string input_file = "input.bin";
    string output_file = "output.bin";

    sort_large_binary_file(input_file, output_file);
    return 0;
}
