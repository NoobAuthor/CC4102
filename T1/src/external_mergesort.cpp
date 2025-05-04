#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <cstdio> // Para remove()

const size_t BLOQUE = 1024;
const size_t M = 4096;
const size_t B_LIMIT = BLOQUE / sizeof(int);

using namespace std;

void phase1_split_and_sort(const string& input_file, vector<string>& temp_files) {
    ifstream infile(input_file, ios::binary);
    if (!infile) {
        cerr << "Error al abrir archivo de entrada." << endl;
        return;
    }

    vector<int> buffer(B_LIMIT);
    int part = 0;

    while (infile.read(reinterpret_cast<char*>(buffer.data()), BLOQUE) || infile.gcount() > 0) {
        size_t items_read = infile.gcount() / sizeof(int);
        sort(buffer.begin(), buffer.begin() + items_read);

        string temp_name = "temp_" + to_string(part++) + ".bin";
        ofstream temp_out(temp_name, ios::binary);
        temp_out.write(reinterpret_cast<char*>(buffer.data()), items_read * sizeof(int));
        temp_out.close();

        temp_files.push_back(temp_name);
    }

    infile.close();
}

void merge_two_files(const string& file1, const string& file2, const string& output_file) {
    ifstream in1(file1, ios::binary);
    ifstream in2(file2, ios::binary);
    ofstream out(output_file, ios::binary);

    vector<int> buffer1(B_LIMIT), buffer2(B_LIMIT), buffer_out(B_LIMIT);
    size_t idx1 = 0, idx2 = 0, out_idx = 0;
    size_t read1 = 0, read2 = 0;

    auto refill_buffer = [](ifstream& in, vector<int>& buffer, size_t& count) {
        in.read(reinterpret_cast<char*>(buffer.data()), BLOQUE);
        count = in.gcount() / sizeof(int);
    };

    refill_buffer(in1, buffer1, read1);
    refill_buffer(in2, buffer2, read2);

    while (idx1 < read1 || idx2 < read2 || !in1.eof() || !in2.eof()) {
        if (idx1 >= read1 && !in1.eof()) {
            refill_buffer(in1, buffer1, read1);
            idx1 = 0;
        }
        if (idx2 >= read2 && !in2.eof()) {
            refill_buffer(in2, buffer2, read2);
            idx2 = 0;
        }

        if (idx1 < read1 && (idx2 >= read2 || buffer1[idx1] <= buffer2[idx2])) {
            buffer_out[out_idx++] = buffer1[idx1++];
        } else if (idx2 < read2) {
            buffer_out[out_idx++] = buffer2[idx2++];
        }

        if (out_idx == B_LIMIT) {
            out.write(reinterpret_cast<char*>(buffer_out.data()), out_idx * sizeof(int));
            out_idx = 0;
        }
    }

    if (out_idx > 0) {
        out.write(reinterpret_cast<char*>(buffer_out.data()), out_idx * sizeof(int));
    }

    in1.close();
    in2.close();
    out.close();
}



void phase2_merge(const vector<string>& temp_files, const string& output_file, int arity = 0) { 
    if (temp_files.size() == 1) {
        remove(output_file.c_str());
        rename(temp_files[0].c_str(), output_file.c_str());
        return;
    }

    vector<string> temp_files2;

    for (size_t i = 0; i < temp_files.size(); i += 2) {
        string file1 = temp_files[i];
        string file2 = (i + 1 < temp_files.size()) ? temp_files[i + 1] : "";
        string temp_out_name = "merge_" + to_string(arity) + "_" + to_string(i / 2) + ".bin";

        if (file2.empty()) {
            // Si no hay segundo archivo, solo lo pasamos directo
            temp_files2.push_back(file1);
        } else {
            merge_two_files(file1, file2, temp_out_name);
            temp_files2.push_back(temp_out_name);

            remove(file1.c_str());
            remove(file2.c_str());
        }
    }

    phase2_merge(temp_files2, output_file, arity + 1);
}



void sort_large_binary_file(const string& input_file, const string& output_file) {
    vector<string> temp_files;
    phase1_split_and_sort(input_file, temp_files);
    phase2_merge(temp_files, output_file);
    cout << "Ordenamiento completo." << endl;
}

int main() {
    string input_file = "input.bin";
    string output_file = "output.bin";

    sort_large_binary_file(input_file, output_file);
    return 0;
}
