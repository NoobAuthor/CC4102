#include "disk_io.hpp"

namespace sorting_project {
size_t io_count = 0;

void reset_io_count() { io_count = 0; }

void read_block(std::ifstream& file, char* buffer) {
  file.read(buffer, BLOCK_SIZE);
  io_count++;
}

void write_block(std::ofstream& file, const char* buffer) {
  file.write(buffer, BLOCK_SIZE);
  io_count++;
}
}  // namespace sorting_project