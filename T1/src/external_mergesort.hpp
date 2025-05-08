#ifndef EXTERNAL_MERGESORT_HPP
#define EXTERNAL_MERGESORT_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <string>
#include <algorithm>
#include <filesystem>
#include <cstdio>
#include <cstdint>

const size_t BLOQUE = 4096;                        // Tamaño del bloque de lectura/escritura en bytes
const size_t M = 51200;                            // Memoria total disponible para uso en bytes
const size_t B_LIMIT = BLOQUE / sizeof(int64_t);   // Cantidad de enteros que caben en un bloque
const size_t M_LIMIT = M / sizeof(int64_t);        // Cantidad de enteros que caben en toda la memoria disponible

namespace fs = std::filesystem;

/**
 * @brief Clase que representa un búfer de entrada para leer archivos binarios por bloques.
 */
struct InputBuffer {
    std::ifstream file;              // Archivo de entrada
    std::vector<int64_t> buffer;     // Búfer de enteros
    size_t idx = 0;                  // Índice actual de lectura en el búfer
    size_t read = 0;                 // Número de elementos leídos en el búfer
    bool finished = false;           // Indica si ya se ha leído todo el archivo

    /**
     * @brief Constructor que abre el archivo y llena el búfer inicial.
     * @param filename Ruta al archivo binario a leer
     */
    InputBuffer(const std::string& filename);

    /**
     * @brief Rellena el búfer desde el archivo.
     */
    void refill();

    /**
     * @brief Verifica si hay más elementos en el búfer.
     * @return true si hay datos disponibles, false si se ha terminado
     */
    bool has_next() const;

    /**
     * @brief Obtiene el elemento actual del búfer.
     * @return Valor entero actual
     */
    int64_t current() const;

    /**
     * @brief Avanza al siguiente elemento en el búfer.
     */
    void advance();
};

/**
 * @brief Clase que maneja múltiples búferes de entrada y permite obtener el siguiente mínimo.
 */
class BufferManager {
public:
    /**
     * @brief Constructor que inicializa todos los búferes con los archivos temporales.
     * @param filenames Lista de nombres de archivos temporales
     */
    BufferManager(const std::vector<std::string>& filenames);

    /**
     * @brief Indica si hay más datos disponibles en cualquiera de los búferes.
     * @return true si al menos un búfer tiene datos disponibles
     */
    bool has_next() const;

    /**
     * @brief Obtiene el menor valor entre todos los búferes y avanza el correspondiente.
     * @return Valor mínimo disponible
     */
    int64_t get_next_min();

private:
    std::vector<InputBuffer> buffers;  // Lista de búferes de entrada
};

/**
 * @brief Lee datos del archivo binario en bloques hasta llenar el búfer.
 * @param in Archivo de entrada
 * @param buffer Vector de enteros para almacenar los datos leídos
 * @param size_buffer Tamaño máximo del búfer en cantidad de enteros
 * @param count Cantidad real de enteros leídos (por referencia)
 */
void refill_buffer(std::ifstream& in, std::vector<int64_t>& buffer, size_t size_buffer, size_t& count);

/**
 * @brief Fase 1 del ordenamiento externo: divide el archivo en bloques, ordena y guarda en archivos temporales.
 * @param input_file Archivo binario de entrada
 * @param temp_files Vector donde se guardarán los nombres de los archivos temporales generados
 */
void phase1_split_and_sort(const std::string& input_file, std::vector<std::string>& temp_files);

/**
 * @brief Fusiona varios archivos binarios ordenados en uno solo.
 * @param input_files Lista de archivos ordenados a fusionar
 * @param output_file Nombre del archivo de salida final
 */
void merge_multiple_files(const std::vector<std::string>& input_files, const std::string& output_file);

/**
 * @brief Fase 2 del ordenamiento externo: fusiona grupos de archivos temporales usando una aridad dada.
 * @param temp_files Archivos temporales generados en la fase 1
 * @param output_file Archivo final con los datos ordenados
 * @param arity Número máximo de archivos que se pueden fusionar a la vez
 */
void phase2_merge(std::vector<std::string> temp_files, const std::string& output_file, int arity);

/**
 * @brief Ejecuta el algoritmo completo de ordenamiento externo.
 * @param input_file Archivo de entrada a ordenar
 * @param output_file Archivo de salida con los datos ordenados
 * @param memBytes Cantidad de memoria disponible en bytes
 * @param arity Grado de fusión (cuántos archivos se pueden combinar en cada paso)
 */
void externalMergesort(const std::string& input_file, const std::string& output_file, size_t memBytes, int arity);

#endif // EXTERNAL_MERGESORT_HPP
