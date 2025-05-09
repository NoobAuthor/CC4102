#include "mergesort.hpp"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>

#include "utils.hpp"

// Constants
const size_t MB = 1024 * 1024;  // 1 MB in bytes
const size_t M = 50 * MB;       // M = 50 MB (max memory for sorting)
const size_t ELEMENT_SIZE = sizeof(int64_t);  // Size of a 64-bit integer

namespace {
struct Run {
  std::string filename;
  std::ifstream file;
  size_t elements_remaining;
  int64_t current_value;
  size_t block_size;
  size_t elements_per_block;
  std::vector<int64_t> buffer;
  size_t buffer_position;
  size_t buffer_size;

  Run(const std::string& fname, size_t elem_count, size_t blk_size);
  bool load_next_block();
  bool next_value();
};

struct RunComparator {
  bool operator()(const std::shared_ptr<Run>& a,
                  const std::shared_ptr<Run>& b) {
    return a->current_value > b->current_value;
  }
};

Run::Run(const std::string& fname, size_t elem_count, size_t blk_size)
    : filename(fname),
      elements_remaining(elem_count),
      block_size(blk_size),
      elements_per_block(blk_size / sizeof(int64_t)),
      buffer(elements_per_block),
      buffer_position(0),
      buffer_size(0) {
  file.open(filename, std::ios::binary);
  if (!file) {
    elements_remaining = 0;
    return;
  }
  load_next_block();
  if (buffer_size > 0) current_value = buffer[0];
}

bool Run::load_next_block() {
  if (elements_remaining == 0) return false;
  size_t elements_to_read = std::min(elements_per_block, elements_remaining);
  file.read(reinterpret_cast<char*>(buffer.data()),
            elements_to_read * sizeof(int64_t));
  buffer_size = file.gcount() / sizeof(int64_t);
  buffer_position = 0;
  disk_reads++;
  return buffer_size > 0;
}

bool Run::next_value() {
  if (elements_remaining == 0) return false;
  buffer_position++;
  elements_remaining--;
  if (buffer_position >= buffer_size && !load_next_block()) return false;
  current_value = buffer[buffer_position];
  return true;
}
}  // namespace

bool external_mergesort(const std::string& input_file,
                        const std::string& output_file, size_t n,
                        size_t block_size, size_t arity) {
  // Reset counters
  disk_reads = 0;
  disk_writes = 0;

  // Calculate sizes
  size_t elements_per_block = block_size / ELEMENT_SIZE;
  size_t max_elements_in_memory = M / ELEMENT_SIZE;

  // Phase 1: Create initial sorted runs
  std::vector<std::string> run_filenames;
  size_t num_runs = (n + max_elements_in_memory - 1) / max_elements_in_memory;

  std::ifstream input(input_file, std::ios::binary);
  if (!input) {
    std::cerr << "Failed to open input file" << std::endl;
    return false;
  }

  for (size_t run_idx = 0; run_idx < num_runs; ++run_idx) {
    // Calculate size of this run
    size_t run_size =
        std::min(max_elements_in_memory, n - run_idx * max_elements_in_memory);

    // Read run into memory
    std::vector<int64_t> run_data(run_size);
    input.read(reinterpret_cast<char*>(run_data.data()),
               run_size * ELEMENT_SIZE);
    disk_reads += (run_size + elements_per_block - 1) / elements_per_block;

    // Sort the run in memory
    std::sort(run_data.begin(), run_data.end());

    // Write sorted run to temporary file
    std::string run_filename = "run_" + std::to_string(run_idx) + ".bin";
    std::ofstream run_file(run_filename, std::ios::binary);
    if (!run_file) {
      std::cerr << "Failed to create run file" << std::endl;
      return false;
    }

    run_file.write(reinterpret_cast<char*>(run_data.data()),
                   run_size * ELEMENT_SIZE);
    disk_writes += (run_size + elements_per_block - 1) / elements_per_block;

    run_file.close();
    run_filenames.push_back(run_filename);
  }

  input.close();

  // Phase 2: Merge runs
  while (run_filenames.size() > 1) {
    std::vector<std::string> new_run_filenames;

    for (size_t i = 0; i < run_filenames.size(); i += arity) {
      // Determine how many runs to merge in this pass
      size_t runs_to_merge = std::min(arity, run_filenames.size() - i);

      // Create output file for merged runs
      std::string merged_filename =
          "merged_" + std::to_string(i / arity) + ".bin";
      std::ofstream merged_file(merged_filename, std::ios::binary);
      if (!merged_file) {
        std::cerr << "Failed to create merged file" << std::endl;
        return false;
      }

      // Set up runs for merging
      std::priority_queue<std::shared_ptr<Run>,
                          std::vector<std::shared_ptr<Run>>, RunComparator>
          run_heap;

      // Calculate elements in each run
      size_t elements_per_run = max_elements_in_memory / runs_to_merge;
      if (i == num_runs - 1) {
        elements_per_run = n - (num_runs - 1) * elements_per_run;
      }

      // Initialize runs
      for (size_t j = 0; j < runs_to_merge; ++j) {
        size_t run_idx = i + j;
        size_t run_elements;

        // Calculate exact number of elements in this run
        if (run_idx < num_runs - 1) {
          run_elements = max_elements_in_memory;
        } else {
          run_elements = n - (num_runs - 1) * max_elements_in_memory;
        }

        std::shared_ptr<Run> run = std::make_shared<Run>(
            run_filenames[run_idx], run_elements, block_size);
        if (run->buffer_size > 0) {
          run_heap.push(run);
        }
      }

      // Create output buffer
      std::vector<int64_t> out_buffer(elements_per_block);
      size_t out_buffer_pos = 0;
      size_t out_block_idx = 0;

      // Merge the runs
      while (!run_heap.empty()) {
        // Get the run with the smallest current value
        std::shared_ptr<Run> min_run = run_heap.top();
        run_heap.pop();

        // Add the value to the output buffer
        out_buffer[out_buffer_pos++] = min_run->current_value;

        // If the buffer is full, write it to the output file
        if (out_buffer_pos == elements_per_block) {
          merged_file.write(reinterpret_cast<char*>(out_buffer.data()),
                            elements_per_block * ELEMENT_SIZE);
          disk_writes++;
          out_buffer_pos = 0;
          out_block_idx++;
        }

        // Get the next value from the run
        if (min_run->next_value()) {
          run_heap.push(min_run);
        }
      }

      // Write any remaining elements in the buffer
      if (out_buffer_pos > 0) {
        merged_file.write(reinterpret_cast<char*>(out_buffer.data()),
                          out_buffer_pos * ELEMENT_SIZE);
        disk_writes++;
      }

      merged_file.close();
      new_run_filenames.push_back(merged_filename);
    }

    // Clean up old run files
    for (const auto& filename : run_filenames) {
      std::remove(filename.c_str());
    }

    run_filenames = new_run_filenames;
  }

  // Rename the final run to the output file
  if (run_filenames.size() == 1) {
    std::remove(output_file.c_str());
    std::rename(run_filenames[0].c_str(), output_file.c_str());
  }

  return true;
}

size_t find_optimal_arity(const std::string& test_file, size_t n,
                          size_t block_size) {
  // Calculate maximum possible arity (how many 64-bit integers fit in a block)
  size_t max_arity = block_size / ELEMENT_SIZE;

  size_t best_arity = 2;  // Default minimal arity
  size_t min_io_operations = std::numeric_limits<size_t>::max();

  // Binary search for the best arity
  size_t low = 2;
  size_t high = max_arity;

  while (low <= high) {
    size_t mid = low + (high - low) / 2;

    // Test the arity
    disk_reads = 0;
    disk_writes = 0;

    std::string output_file = "test_sorted_" + std::to_string(mid) + ".bin";
    external_mergesort(test_file, output_file, n, block_size, mid);

    size_t total_io = disk_reads + disk_writes;

    std::cout << "Tested arity " << mid << ": " << total_io << " I/O operations"
              << std::endl;

    // Check if this is better than our current best
    if (total_io < min_io_operations) {
      min_io_operations = total_io;
      best_arity = mid;

      // Continue searching for better values on the left side
      high = mid - 1;
    } else {
      // Look on the right side
      low = mid + 1;
    }

    // Clean up the output file
    std::remove(output_file.c_str());
  }

  return best_arity;
}