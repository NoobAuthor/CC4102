#include "utils.hpp"
#include <iostream>

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
