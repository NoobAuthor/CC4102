#include "quicksort.hpp"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <random>
#include <vector>

#include "utils.hpp"
// Constants
const size_t MB = 1024 * 1024;  // 1 MB in bytes
const size_t M = 50 * MB;       // M = 50 MB (max memory for sorting)
const size_t ELEMENT_SIZE = sizeof(int64_t);  // Size of a 64-bit integer

std::vector<size_t> partition_with_pivots(
    const std::string& input_file, const std::vector<std::string>& output_files,
    const std::vector<int64_t>& pivots, size_t n, size_t block_size) {
  size_t num_partitions = pivots.size() + 1;
  size_t elements_per_block = block_size / ELEMENT_SIZE;

  // Open input file
  std::ifstream input(input_file, std::ios::binary);
  if (!input) {
    std::cerr << "Failed to open input file for partitioning" << std::endl;
    return std::vector<size_t>(num_partitions, 0);
  }

  // Open output files
  std::vector<std::ofstream> outputs(num_partitions);
  for (size_t i = 0; i < num_partitions; ++i) {
    outputs[i].open(output_files[i], std::ios::binary);
    if (!outputs[i]) {
      std::cerr << "Failed to open output file for partition " << i
                << std::endl;
      return std::vector<size_t>(num_partitions, 0);
    }
  }

  // Buffers for reading and writing
  std::vector<int64_t> in_buffer(elements_per_block);
  std::vector<std::vector<int64_t>> out_buffers(num_partitions);
  std::vector<size_t> out_buffer_pos(num_partitions, 0);
  std::vector<size_t> partition_sizes(num_partitions, 0);

  for (size_t i = 0; i < num_partitions; ++i) {
    out_buffers[i].resize(elements_per_block);
  }

  // Process the input file in blocks
  size_t elements_read = 0;

  while (elements_read < n) {
    size_t elements_to_read = std::min(elements_per_block, n - elements_read);

    input.read(reinterpret_cast<char*>(in_buffer.data()),
               elements_to_read * ELEMENT_SIZE);
    size_t elements_actually_read = input.gcount() / ELEMENT_SIZE;
    disk_reads++;

    if (elements_actually_read == 0) {
      break;
    }

    // Process each element in the block
    for (size_t i = 0; i < elements_actually_read; ++i) {
      int64_t value = in_buffer[i];

      // Find the appropriate partition for this value
      size_t partition = 0;
      while (partition < pivots.size() && value >= pivots[partition]) {
        partition++;
      }

      // Add to the appropriate output buffer
      out_buffers[partition][out_buffer_pos[partition]++] = value;
      partition_sizes[partition]++;

      // If the buffer is full, write it to the output file
      if (out_buffer_pos[partition] == elements_per_block) {
        outputs[partition].write(
            reinterpret_cast<char*>(out_buffers[partition].data()),
            elements_per_block * ELEMENT_SIZE);
        disk_writes++;
        out_buffer_pos[partition] = 0;
      }
    }

    elements_read += elements_actually_read;
  }

  // Write any remaining elements in the buffers
  for (size_t i = 0; i < num_partitions; ++i) {
    if (out_buffer_pos[i] > 0) {
      outputs[i].write(reinterpret_cast<char*>(out_buffers[i].data()),
                       out_buffer_pos[i] * ELEMENT_SIZE);
      disk_writes++;
    }
    outputs[i].close();
  }

  input.close();

  return partition_sizes;
}

void in_memory_quicksort(std::vector<int64_t>& arr, int left, int right) {
  if (left < right) {
    // Choose pivot (median of three)
    int mid = left + (right - left) / 2;
    int64_t pivot;

    if (arr[left] > arr[mid]) {
      std::swap(arr[left], arr[mid]);
    }
    if (arr[left] > arr[right]) {
      std::swap(arr[left], arr[right]);
    }
    if (arr[mid] > arr[right]) {
      std::swap(arr[mid], arr[right]);
    }

    pivot = arr[mid];
    std::swap(arr[mid], arr[right - 1]);

    // Partition
    int i = left;
    int j = right - 1;

    while (true) {
      while (arr[++i] < pivot) {
      }
      while (arr[--j] > pivot) {
      }

      if (i >= j) {
        break;
      }

      std::swap(arr[i], arr[j]);
    }

    std::swap(arr[i], arr[right - 1]);

    // Recursively sort the two partitions
    in_memory_quicksort(arr, left, i - 1);
    in_memory_quicksort(arr, i + 1, right);
  }
}

bool external_quicksort(const std::string& input_file,
                        const std::string& output_file, size_t n,
                        size_t block_size, size_t num_pivots) {
  // Reset counters
  disk_reads = 0;
  disk_writes = 0;

  // If the array fits in memory, sort it directly
  size_t elements_per_block = block_size / ELEMENT_SIZE;
  size_t max_elements_in_memory = M / ELEMENT_SIZE;

  if (n <= max_elements_in_memory) {
    // Read the entire array into memory
    std::vector<int64_t> data(n);
    std::ifstream input(input_file, std::ios::binary);
    if (!input) {
      std::cerr << "Failed to open input file" << std::endl;
      return false;
    }

    input.read(reinterpret_cast<char*>(data.data()), n * ELEMENT_SIZE);
    disk_reads += (n + elements_per_block - 1) / elements_per_block;
    input.close();

    // Sort in memory
    in_memory_quicksort(data, 0, n - 1);

    // Write back to output file
    std::ofstream output(output_file, std::ios::binary);
    if (!output) {
      std::cerr << "Failed to create output file" << std::endl;
      return false;
    }

    output.write(reinterpret_cast<char*>(data.data()), n * ELEMENT_SIZE);
    disk_writes += (n + elements_per_block - 1) / elements_per_block;
    output.close();

    return true;
  }

  // Choose pivots
  std::vector<int64_t> pivots;
  std::vector<int64_t> sample_buffer(elements_per_block);

  // Read a random block
  std::ifstream input(input_file, std::ios::binary);
  if (!input) {
    std::cerr << "Failed to open input file for pivot selection" << std::endl;
    return false;
  }

  size_t num_blocks = (n + elements_per_block - 1) / elements_per_block;
  size_t random_block = std::rand() % num_blocks;

  input.seekg(random_block * block_size);
  input.read(reinterpret_cast<char*>(sample_buffer.data()), block_size);
  size_t elements_read = input.gcount() / ELEMENT_SIZE;
  disk_reads++;
  input.close();

  // Sort the sample to select pivots
  std::sort(sample_buffer.begin(), sample_buffer.begin() + elements_read);

  // Select pivots (evenly spaced)
  size_t num_partitions = num_pivots + 1;
  for (size_t i = 1; i < num_partitions; ++i) {
    size_t idx = i * elements_read / num_partitions;
    if (idx < elements_read) {
      pivots.push_back(sample_buffer[idx]);
    }
  }

  // If we couldn't get enough pivots, adjust
  if (pivots.size() < num_pivots) {
    std::cerr << "Warning: Could only select " << pivots.size()
              << " pivots instead of " << num_pivots << std::endl;
  }

  // Create temporary files for partitions
  std::vector<std::string> partition_files;
  for (size_t i = 0; i < pivots.size() + 1; ++i) {
    partition_files.push_back("partition_" + std::to_string(i) + ".bin");
  }

  // Partition the input file
  std::vector<size_t> partition_sizes =
      partition_with_pivots(input_file, partition_files, pivots, n, block_size);

  // Recursively sort each partition
  for (size_t i = 0; i < partition_files.size(); ++i) {
    if (partition_sizes[i] > 0) {
      std::string sorted_partition =
          "sorted_partition_" + std::to_string(i) + ".bin";

      external_quicksort(partition_files[i], sorted_partition,
                         partition_sizes[i], block_size, num_pivots);

      // Replace original partition file with sorted one
      std::remove(partition_files[i].c_str());
      std::rename(sorted_partition.c_str(), partition_files[i].c_str());
    }
  }

  // Concatenate the sorted partitions
  std::ofstream output(output_file, std::ios::binary);
  if (!output) {
    std::cerr << "Failed to create output file for concatenation" << std::endl;
    return false;
  }

  std::vector<int64_t> concat_buffer(elements_per_block);

  for (size_t i = 0; i < partition_files.size(); ++i) {
    if (partition_sizes[i] > 0) {
      std::ifstream part_file(partition_files[i], std::ios::binary);
      if (!part_file) {
        std::cerr << "Failed to open partition file for concatenation"
                  << std::endl;
        return false;
      }

      size_t elements_remaining = partition_sizes[i];

      while (elements_remaining > 0) {
        size_t elements_to_read =
            std::min(elements_per_block, elements_remaining);

        part_file.read(reinterpret_cast<char*>(concat_buffer.data()),
                       elements_to_read * ELEMENT_SIZE);
        disk_reads++;

        output.write(reinterpret_cast<char*>(concat_buffer.data()),
                     elements_to_read * ELEMENT_SIZE);
        disk_writes++;

        elements_remaining -= elements_to_read;
      }

      part_file.close();
      std::remove(partition_files[i].c_str());
    }
  }

  output.close();

  return true;
}