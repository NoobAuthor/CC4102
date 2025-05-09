#ifndef UTILS_HPP
#define UTILS_HPP

#include <fstream>
#include <string>

extern size_t disk_reads;
extern size_t disk_writes;

bool read_block(std::ifstream& file, void* buffer, size_t block_size,
                size_t offset);
bool write_block(std::ofstream& file, const void* buffer, size_t block_size,
                 size_t offset);
bool generate_random_array(const std::string& filename, size_t n,
                           size_t block_size);
bool check_sorted(const std::string& filename, size_t n, size_t block_size);

#endif