#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <filesystem>
#include <cstdio>
#include <cstdint>  // para int64_t

const size_t BLOQUE = 4096
const size_t M = 51200
const size_t B_LIMIT = BLOQUE / sizeof(int64_t); 
const size_t M_LIMIT = 51200/ sizeof(int64_t); 

using namespace std;
namespace fs = std::filesystem;

struct InputBuffer {
    ifstream file;
    vector<int64_t> buffer;
    size_t idx = 0;
    size_t read = 0;
    bool finished = false;

    InputBuffer(const string& filename)
        : file(filename, ios::binary), buffer(B_LIMIT) {
        if (!file) {
            cerr << "Error abriendo archivo: " << filename << endl;
            finished = true;
        } else {
            refill();
        }
    }

    void refill() {
        if (!file || file.eof()) {
            finished = true;
            return;
        }
        file.read(reinterpret_cast<char*>(buffer.data()), B_LIMIT * sizeof(int64_t));
        read = file.gcount() / sizeof(int64_t);
        idx = 0;
        if (read == 0) finished = true;
    }

    bool has_next() const {
        return !finished && idx < read;
    }

    int64_t current() const {
        return buffer[idx];
    }

    void advance() {
        ++idx;
        if (idx >= read) {
            refill();
        }
    }
};

class BufferManager {
    public:
        BufferManager(const vector<string>& filenames) {
            for (const auto& name : filenames) {
                buffers.emplace_back(name);
            }
        }
    
        bool has_next() const {
            for (const auto& buf : buffers) {
                if (buf.has_next()) return true;
            }
            return false;
        }
    
        int64_t get_next_min() {
            int min_idx = -1;
            int64_t min_val = 0;
    
            for (size_t i = 0; i < buffers.size(); ++i) {
                if (buffers[i].has_next()) {
                    if (min_idx == -1 || buffers[i].current() < min_val) {
                        min_idx = i;
                        min_val = buffers[i].current();
                    }
                }
            }
    
            if (min_idx == -1) {
                throw runtime_error("No hay más elementos disponibles.");
            }
    
            buffers[min_idx].advance();
            return min_val;
        }
    
    private:
        vector<InputBuffer> buffers;
    };
    


void refill_buffer(ifstream& in, vector<int64_t>& buffer, size_t size_buffer, size_t& count) {
    count = 0;
    size_t total_bytes_needed = size_buffer * sizeof(int64_t);

    while (count * sizeof(int64_t) < total_bytes_needed && in) {
        size_t bytes_to_read = min(BLOQUE, total_bytes_needed - count * sizeof(int64_t));
        in.read(reinterpret_cast<char*>(buffer.data()) + count * sizeof(int64_t), bytes_to_read);
        size_t bytes_read = in.gcount();

        count += bytes_read / sizeof(int64_t);

        if (bytes_read < bytes_to_read) {
            // Si leímos menos de lo que pedimos, probablemente llegamos al final del archivo
            break;
        }
    }
}


void phase1_split_and_sort(const string& input_file, vector<string>& temp_files) {
    ifstream infile(input_file, ios::binary);
    if (!infile) {
        cerr << "Error al abrir archivo de entrada." << endl;
        return;
    }

    vector<int64_t> buffer(M_LIMIT);
    int part = 0;
    size_t items_read = 0;

    refill_buffer(infile, buffer, M_LIMIT, items_read);

    while (items_read > 0) {
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

        refill_buffer(infile, buffer, M_LIMIT, items_read);
    }
}


void merge_multiple_files(const vector<string>& input_files, const string& output_file) {
    BufferManager manager(input_files);
    ofstream out(output_file, ios::binary);
    if (!out) {
        cerr << "Error al abrir archivo de salida." << endl;
        return;
    }

    vector<int64_t> output_buffer(B_LIMIT);
    size_t out_idx = 0;

    while (manager.has_next()) {
        int64_t val = manager.get_next_min();
        output_buffer[out_idx++] = val;

        if (out_idx == B_LIMIT) {
            out.write(reinterpret_cast<char*>(output_buffer.data()), out_idx * sizeof(int64_t));
            out_idx = 0;
        }
    }

    if (out_idx > 0) {
        out.write(reinterpret_cast<char*>(output_buffer.data()), out_idx * sizeof(int64_t));
    }
}


void phase2_merge(vector<string> temp_files, const string& output_file, int arity) {
    while (temp_files.size() > 1) {
        vector<string> next_round;

        for (size_t i = 0; i < temp_files.size(); i += arity) {
            // Obtener el subvector de archivos a fusionar
            vector<string> group;
            for (size_t j = i; j < i + arity && j < temp_files.size(); ++j) {
                group.push_back(temp_files[j]);
            }

            if (group.size() == 1) {
                next_round.push_back(group[0]); // Solo uno, no necesita fusión
            } else {
                string merged_file = "merge_" + to_string(arity) + "_" + to_string(i / arity) + ".bin";
                merge_multiple_files(group, merged_file);

                // Eliminar archivos temporales
                for (const auto& file : group) {
                    fs::remove(file);
                }

                next_round.push_back(merged_file);
            }
        }

        temp_files = move(next_round);
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
