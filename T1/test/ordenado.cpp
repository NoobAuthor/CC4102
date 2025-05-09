#include <iostream>
#include <fstream>
#include <cstdint>

bool archivoOrdenadoAscendente(const std::string& nombreArchivo) {
    std::ifstream archivo(nombreArchivo, std::ios::binary);
    if (!archivo) {
        std::cerr << "No se pudo abrir el archivo.\n";
        return false;
    }

    int64_t actual, anterior;
    // Leer el primer valor
    if (!archivo.read(reinterpret_cast<char*>(&anterior), sizeof(int64_t))) {
        std::cout << "Archivo vacío.\n";
        return true;
    }
    bool ordenado = true;

    while (archivo.read(reinterpret_cast<char*>(&actual), sizeof(int64_t))) {
        std::cout << actual << "\n";
        if (actual < anterior) {
            ordenado = false;
        }
        anterior = actual;
    }

    return ordenado;
}


int main() {
    std::string ruta = "output.bin";
    if (archivoOrdenadoAscendente(ruta)) {
        std::cout << "El archivo está ordenado ascendentemente.\n";
    } else {
        std::cout << "El archivo NO está ordenado ascendentemente.\n";
    }
    return 0;
}
