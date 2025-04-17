#include "utils.hpp"

#include <chrono>
#include <cstring>
#include <iostream>
#include <random>

// Global counter for disk accesses
int64_t g_disk_access_count = 0;

void write_block(FILE* file, int64_t* buffer, size_t block_size, long position) {
    fseek(file, position * block_size, SEEK_SET);

  size_t written = fwrite(buffer, 1, block_size, file);

  g_disk_access_count++;

  if (written != block_size) {
    std::cerr << "Error writing block: " << written
              << " bytes written instead of " << block_size << std::endl;
  }
}

void read_block(FILE* file, int64_t* buffer, size_t block_size, long position) {
  fseek(file, position * block_size, SEEK_SET);

  size_t read = fread(buffer, 1, block_size, file);

  g_disk_access_count++;

  if (read != block_size) {
    memset(((char*)buffer) + read, 0, block_size - read);
  }
}

void generate_random_array(const char* filename, size_t size,
                           size_t block_size) {
  FILE* file = fopen(filename, "wb");
  if (!file) {
    std::cerr << "Could not open file for writing: " << filename << std::endl;
    return;
  }
  std::random_device rd;
  std::mt19937_64 gen(rd());
  std::uniform_int_distribution<int64_t> dist;

  size_t ints_per_block = block_size / sizeof(int64_t);

  int64_t* buffer = new int64_t[ints_per_block];

  size_t num_blocks = (size + ints_per_block - 1) / ints_per_block;

  for (size_t block = 0; block < num_blocks; block++) {
    size_t block_ints = (block == num_blocks - 1 && size % ints_per_block != 0)
                            ? size % ints_per_block
                            : ints_per_block;

    for (size_t i = 0; i < block_ints; i++) {
      buffer[i] = dist(gen);
    }
    write_block(file, buffer, block_size, block);
  }

  delete[] buffer;
  fclose(file);
}

bool is_sorted(const char* filename, size_t size, size_t block_size) {
  FILE* file = fopen(filename, "rb");
  if (!file) {
    std::cerr << "Could not open file for reading: " << filename << std::endl;
    return false;
  }

  size_t ints_per_block = block_size / sizeof(int64_t);

  int64_t* current_block = new int64_t[ints_per_block];

  size_t num_blocks = (size + ints_per_block - 1) / ints_per_block;

  bool sorted = true;
  int64_t last_value = INT64_MIN;

  for (size_t block = 0; block < num_blocks && sorted; block++) {
    read_block(file, current_block, block_size, block);

    size_t block_ints = (block == num_blocks - 1 && size % ints_per_block != 0)
                            ? size % ints_per_block
                            : ints_per_block;
    for (size_t i = 0; i < block_ints && sorted; i++) {
      if (current_block[i] < last_value) {
        sorted = false;
        break;
      }
      last_value = current_block[i];
    }
  }

  delete[] current_block;
  fclose(file);

  return sorted;
}

void reset_disk_access_counter() { g_disk_access_count = 0; }

int64_t get_disk_access_count() { return g_disk_access_count; }

double measure_execution_time(std::function<void()> algorithm) {
  auto start = std::chrono::high_resolution_clock::now();
  algorithm();
  auto end = std::chrono::high_resolution_clock::now();

  std::chrono::duration<double> duration = end - start;
  return duration.count();
}

size_t integers_per_block(size_t block_size) {
  return block_size / sizeof(int64_t);
}