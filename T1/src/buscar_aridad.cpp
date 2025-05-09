#include <algorithm>
#include <cstdint>  // para int64_t
#include <chrono>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <string>
#include <vector>

const size_t BLOQUE = 4*1024;
const size_t M = 50*1024*1024;
const size_t B_LIMIT = BLOQUE / sizeof(int64_t); 
const size_t M_LIMIT = M/ sizeof(int64_t); 
size_t total_reads = 0;
size_t total_writes = 0;



using namespace std;
namespace fs = std::filesystem;

void generate_random_blocks(const string& filename, size_t total_numbers, size_t block_size, int64_t max_range) {
    ofstream out(filename, ios::binary);
    if (!out) {
        cerr << "Error al crear " << filename << endl;
        return;
    }

    random_device rd;
    mt19937_64 g(rd());  // usa generador de 64 bits para acompañar int64_t
    uniform_int_distribution<int64_t> range_dist(0, max_range - static_cast<int64_t>(block_size));

    size_t written = 0;
    while (written < total_numbers) {
        size_t current_block = min(block_size, total_numbers - written);

        // Selecciona aleatoriamente un inicio de rango para este bloque
        int64_t start = range_dist(g);
        int64_t end = start + static_cast<int64_t>(current_block);

        vector<int64_t> block(current_block);
        iota(block.begin(), block.end(), start);

        shuffle(block.begin(), block.end(), g);

        out.write(reinterpret_cast<const char*>(block.data()), block.size() * sizeof(int64_t));
        written += current_block;

        cout << "Bloque generado [" << start << ", " << end << ") con " << current_block
             << " números. Total escrito: " << written << endl;
    }

    out.close();
    cout << "Archivo " << filename << " generado con " << total_numbers << " números únicos en bloques aleatorios." << endl;
}

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
        total_reads++;
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
        total_reads++;
        size_t bytes_read = in.gcount();

        count += bytes_read / sizeof(int64_t);

        if (bytes_read < bytes_to_read) {
            // Si leímos menos de lo que pedimos, probablemente llegamos al final del archivo
            break;
        }
    }
}

void write_buffer_in_blocks(ofstream& out, const vector<int64_t>& buffer, size_t items_to_write) {
    size_t bytes_to_write = items_to_write * sizeof(int64_t);
    size_t offset = 0;

    while (offset < bytes_to_write) {
        size_t chunk = min(BLOQUE, bytes_to_write - offset);
        out.write(reinterpret_cast<const char*>(buffer.data()) + offset, chunk);
        total_writes++;

        if (!out) {
            cerr << "Error al escribir bloque en archivo temporal." << endl;
            break;
        }

        offset += chunk;
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
        write_buffer_in_blocks(temp_out, buffer, items_read);
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
            out.write(reinterpret_cast<char*>(output_buffer.data()),BLOQUE);
            total_writes++;

            out_idx = 0;
        }
    }

    if (out_idx > 0) {
        out.write(reinterpret_cast<char*>(output_buffer.data()), out_idx * sizeof(int64_t));
        total_writes++;

    }

    out.close();
    
    if (!out) {
        cerr << "Error al escribir el archivo de salida." << endl;
        return;
    }
    
}


void phase2_merge(vector<string> temp_files, const string& output_file, int arity) {
    while (temp_files.size() > 1) {
        vector<string> next_round;
        cout << "Aquí comienza una ronda " << temp_files.size() << endl;

        for (size_t i = 0; i < temp_files.size(); i += arity) {
            vector<string> group;
            for (size_t j = i; j < i + arity && j < temp_files.size(); ++j) {
                group.push_back(temp_files[j]);
            }

            if (group.size() == 1) {
                next_round.push_back(group[0]);
            } else {
                string merged_file = "merge_" + to_string(arity) + "_" + to_string(i / arity) + "_" + to_string(temp_files.size()) + ".bin";


                merge_multiple_files(group, merged_file);

                if (!fs::exists(merged_file)) {
                    cerr << "Error: No se creó el archivo de fusión: " << merged_file << endl;
                    exit(1);
                }

                // Eliminar archivos solo si la fusión tuvo éxito
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

        if (fs::exists(temp_files[0])) {
            fs::rename(temp_files[0], output_file);
        } else {
            cerr << "Error: Archivo final no encontrado para renombrar: " << temp_files[0] << endl;
            exit(1);
        }
    } else {
        cerr << "Error: No hay archivos temporales al final de phase2_merge." << endl;
        exit(1);
    }
}



void externalMergesort(const string& input_file, const string& output_file, size_t memBytes, int arity) {
    vector<string> temp_files;
    phase1_split_and_sort(input_file, temp_files);
    if (!temp_files.empty()) {
        phase2_merge(temp_files, output_file, arity);
        cout << "Ordenamiento completo." << endl;
    }
}


int buscar_a_optimo(const string& input_file, const string& output_file, int min_a, int max_a, const string& log_csv) {
    ofstream log(log_csv);
    log << "a,Promedio_Lecturas,Promedio_Escrituras,Costo_Total_I/O\n";
    const string input_file2 = "temp_input2.bin";
    int mejor_a = min_a;
    size_t mejor_costo = SIZE_MAX;
    generate_random_blocks(input_file, 60 * M_LIMIT, B_LIMIT, 1LL << 60); 

    auto evaluar = [&](int a) {
        fs::copy_file(input_file, input_file2 , fs::copy_options::overwrite_existing);
        total_reads = total_writes = 0;
        externalMergesort(input_file2, output_file, M, a);
        fs::remove(input_file2);
        fs::remove(output_file);

        size_t costo = total_reads + total_writes;
        log << a << "," << (total_reads) << "," << (total_writes) << "," << (costo) << "\n";

        return costo;
    };

    while (max_a - min_a > 3) {
        int m1 = min_a + (max_a - min_a) / 3;
        int m2 = max_a - (max_a - min_a) / 3;

        size_t costo_m1 = evaluar(m1);
        size_t costo_m2 = evaluar(m2);

        if (costo_m1 < costo_m2) {
            max_a = m2;
            if (costo_m1 < mejor_costo) {
                mejor_costo = costo_m1;
                mejor_a = m1;
            }
        } else {
            min_a = m1;
            if (costo_m2 < mejor_costo) {
                mejor_costo = costo_m2;
                mejor_a = m2;
            }
        }
    }

    for (int a = min_a; a <= max_a; ++a) {
        size_t costo = evaluar(a);
        if (costo < mejor_costo) {
            mejor_costo = costo;
            mejor_a = a;
        }
    }

    log.close();
    cout << "Mejor valor de a encontrado: " << mejor_a << " con costo total aproximado: " << mejor_costo << " I/Os promedio\n" << endl;
    return mejor_a;
}




int main() {
    const string input_file = "temp_input.bin";
    const string output_file = "temp_output.bin";
    const string log_csv = "busqueda_a.csv";

    int b = BLOQUE / sizeof(int64_t);  // Máximo valor posible para a
    int mejor_a = buscar_a_optimo(input_file, output_file, 2, b, log_csv);

    cout << "Valor óptimo de a para usar en Quicksort externo: " << mejor_a << endl;
    return 0;
}


