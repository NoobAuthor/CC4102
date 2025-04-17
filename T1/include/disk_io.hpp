#pragma once
#include <fstream>
#include <string>

namespace sorting_project {
constexpr size_t BLOCK_SIZE = 4096;
constexpr size_t ELEMENT_SIZE = 8;
constexpr size_t ELEMENTS_PER_BLOCK = BLOCK_SIZE / ELEMENT_SIZE;

extern size_t io_count;

// Resets the global I/O counter.
void reset_io_count();

// Reads block of BLOCK_SIZE bytes from file into buffer.
void read_block(std::ifstream &file, char *buffer);

// Writes block of BLOCK_SIZE bytes from buffer to file.
void write_block(std::ofstream &file, const char *buffer);
}  // namespace sorting_project
