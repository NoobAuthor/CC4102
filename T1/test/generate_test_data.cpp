#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <cstdint>  // para int64_t

using namespace std;

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

int main() {
    const string filename = "input.bin";
    const size_t total_numbers = 100;   // 10 millones (ajusta según necesidad real)
    const size_t block_size = 1024;     // 100 mil por bloque
    const int64_t max_range = 10000000000;  // valores hasta 10 mil millones

    generate_random_blocks(filename, total_numbers, block_size, max_range);

    return 0;
}
