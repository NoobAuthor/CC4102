#include "external_quicksort.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>
#include <random>
#include <ctime>

void external_quicksort(const char* input_file, const char* output_file, 
                      size_t size, size_t block_size, 
                      size_t memory_limit, int num_pivots) {
    // Wrapper function that calls the recursive implementation
    external_quicksort_recursive(input_file, output_file, size, block_size, 
                               memory_limit, num_pivots, 0);
}

void external_quicksort_recursive(const char* input_file, const char* output_file, 
                               size_t size, size_t block_size, 
                               size_t memory_limit, int num_pivots, int depth) {
    // Calculate how many integers can fit in memory at once
    size_t ints_per_block = integers_per_block(block_size);
    size_t max_ints_in_memory = memory_limit / sizeof(int64_t);
    
    // If the data fits in memory, sort it in memory and write it back
    if (size <= max_ints_in_memory) {
        FILE* input = fopen(input_file, "rb");
        if (!input) {
            std::cerr << "Could not open input file: " << input_file << std::endl;
            return;
        }
        
        int64_t* buffer = new int64_t[size];
        size_t blocks_to_read = (size + ints_per_block - 1) / ints_per_block;
        
        // Read the entire file into memory
        size_t ints_read = 0;
        for (size_t b = 0; b < blocks_to_read && ints_read < size; b++) {
            int64_t block_buffer[ints_per_block];
            read_block(input, block_buffer, block_size, b);
            
            size_t block_ints = std::min(ints_per_block, size - ints_read);
            memcpy(buffer + ints_read, block_buffer, block_ints * sizeof(int64_t));
            ints_read += block_ints;
        }
        
        fclose(input);
        
        // Sort the data in memory
        std::sort(buffer, buffer + size);
        
        // Write the sorted data back to disk
        FILE* output = fopen(output_file, "wb");
        if (!output) {
            std::cerr << "Could not open output file: " << output_file << std::endl;
            delete[] buffer;
            return;
        }
        
        size_t ints_written = 0;
        for (size_t b = 0; b < blocks_to_read && ints_written < size; b++) {
            size_t block_ints = std::min(ints_per_block, size - ints_written);
            int64_t block_buffer[ints_per_block];
            
            memcpy(block_buffer, buffer + ints_written, block_ints * sizeof(int64_t));
            
            if (block_ints < ints_per_block) {
                memset(block_buffer + block_ints, 0, 
                      (ints_per_block - block_ints) * sizeof(int64_t));
            }
            
            write_block(output, block_buffer, block_size, b);
            ints_written += block_ints;
        }
        
        fclose(output);
        delete[] buffer;
        return;
    }
    
    // If we need to partition, select pivots and create partitions
    int64_t* pivots = new int64_t[num_pivots];
    select_pivots(input_file, size, block_size, num_pivots, pivots);
    
    // Number of partitions is one more than the number of pivots
    int num_partitions = num_pivots + 1;
    
    // Create temporary files for each partition
    char** partition_files = new char*[num_partitions];
    for (int i = 0; i < num_partitions; i++) {
        partition_files[i] = new char[100];
        sprintf(partition_files[i], "partition_%d_%d.bin", depth, i);
    }
    
    // Keep track of partition sizes
    size_t* partition_sizes = new size_t[num_partitions]();
    
    // Partition the file based on the pivots
    partition_file(input_file, const_cast<const char**>(partition_files), 
                  partition_sizes, size, block_size, memory_limit, 
                  pivots, num_pivots);
    
    // Recursively sort each partition
    for (int i = 0; i < num_partitions; i++) {
        if (partition_sizes[i] > 0) {
            char sorted_partition[100];
            sprintf(sorted_partition, "sorted_partition_%d_%d.bin", depth, i);
            
            external_quicksort_recursive(partition_files[i], sorted_partition, 
                                      partition_sizes[i], block_size, 
                                      memory_limit, num_pivots, depth + 1);
            
            // Replace the partition file with its sorted version
            remove(partition_files[i]);
            rename(sorted_partition, partition_files[i]);
        }
    }
    
    // Concatenate the sorted partitions to form the output file
    FILE* output = fopen(output_file, "wb");
    if (!output) {
        std::cerr << "Could not open output file: " << output_file << std::endl;
        
        // Clean up resources
        delete[] pivots;
        for (int i = 0; i < num_partitions; i++) {
            delete[] partition_files[i];
        }
        delete[] partition_files;
        delete[] partition_sizes;
        
        return;
    }
    
    size_t output_position = 0;
    int64_t buffer[ints_per_block];
    
    // Append each partition to the output file
    for (int i = 0; i < num_partitions; i++) {
        if (partition_sizes[i] > 0) {
            FILE* partition = fopen(partition_files[i], "rb");
            if (!partition) {
                std::cerr << "Could not open partition file: " << partition_files[i] << std::endl;
                continue;
            }
            
            size_t blocks_to_read = (partition_sizes[i] + ints_per_block - 1) / ints_per_block;
            
            for (size_t b = 0; b < blocks_to_read; b++) {
                read_block(partition, buffer, block_size, b);
                write_block(output, buffer, block_size, output_position++);
            }
            
            fclose(partition);
            remove(partition_files[i]);  // Clean up temporary files
        }
    }
    
    fclose(output);
    
    // Clean up resources
    delete[] pivots;
    for (int i = 0; i < num_partitions; i++) {
        delete[] partition_files[i];
    }
    delete[] partition_files;
    delete[] partition_sizes;
}

void select_pivots(const char* input_file, size_t size, size_t block_size, 
                  int num_pivots, int64_t* pivots) {
    if (num_pivots <= 0) return;
    
    FILE* input = fopen(input_file, "rb");
    if (!input) {
        std::cerr << "Could not open input file: " << input_file << std::endl;
        return;
    }
    
    // Initialize random generator
    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)));
    std::uniform_int_distribution<size_t> dist(0, size - 1);
    
    size_t ints_per_block = integers_per_block(block_size);
    std::vector<int64_t> sample;
    
    // Select a random block from the file
    size_t num_blocks = (size + ints_per_block - 1) / ints_per_block;
    size_t random_block = dist(rng) % num_blocks;
    
    int64_t buffer[ints_per_block];
    read_block(input, buffer, block_size, random_block);
    
    // Extract integers from the block to use as samples
    size_t block_size_integers = std::min(ints_per_block, size - random_block * ints_per_block);
    for (size_t i = 0; i < block_size_integers; i++) {
        sample.push_back(buffer[i]);
    }
    
    fclose(input);
    
    // If we don't have enough samples, choose random ones
    if (sample.size() < static_cast<size_t>(num_pivots)) {
        // This is a fallback, should not happen with reasonably sized blocks
        std::cerr << "Warning: Not enough samples for pivots" << std::endl;
        while (sample.size() < static_cast<size_t>(num_pivots)) {
            sample.push_back(sample[dist(rng) % sample.size()]);
        }
    }
    
    // Sort the sample and select pivots
    std::sort(sample.begin(), sample.end());
    
    // Choose pivots to divide the sample into equal-sized segments
    for (int i = 0; i < num_pivots; i++) {
        size_t idx = (i + 1) * sample.size() / (num_pivots + 1);
        pivots[i] = sample[idx];
    }
}

void partition_file(const char* input_file, const char** partition_files, 
                   size_t* partition_sizes, size_t size, size_t block_size, 
                   size_t memory_limit, int64_t* pivots, int num_pivots) {
    FILE* input = fopen(input_file, "rb");
    if (!input) {
        std::cerr << "Could not open input file: " << input_file << std::endl;
        return;
    }
    
    int num_partitions = num_pivots + 1;
    FILE** partition_outputs = new FILE*[num_partitions];
    
    // Open all partition files for writing
    for (int i = 0; i < num_partitions; i++) {
        partition_outputs[i] = fopen(partition_files[i], "wb");
        if (!partition_outputs[i]) {
            std::cerr << "Could not open partition file: " << partition_files[i] << std::endl;
            
            // Close previously opened files
            for (int j = 0; j < i; j++) {
                fclose(partition_outputs[j]);
            }
            
            fclose(input);
            delete[] partition_outputs;
            return;
        }
        
        partition_sizes[i] = 0;  // Initialize partition sizes
    }
    
    size_t ints_per_block = integers_per_block(block_size);
    size_t max_blocks_in_memory = memory_limit / block_size;
    
    // Allocate a buffer for reading blocks
    if (max_blocks_in_memory == 0) max_blocks_in_memory = 1;
    size_t buffer_size = std::min(max_blocks_in_memory, (size + ints_per_block - 1) / ints_per_block);
    int64_t* buffer = new int64_t[buffer_size * ints_per_block];
    
    // For writing partitions
    size_t* partition_positions = new size_t[num_partitions]();
    int64_t** partition_buffers = new int64_t*[num_partitions];
    for (int i = 0; i < num_partitions; i++) {
        partition_buffers[i] = new int64_t[ints_per_block];
        memset(partition_buffers[i], 0, ints_per_block * sizeof(int64_t));
    }
    size_t* partition_buffer_positions = new size_t[num_partitions]();
    
    // Read the input file in chunks
    size_t num_blocks = (size + ints_per_block - 1) / ints_per_block;
    for (size_t block_start = 0; block_start < num_blocks;) {
        // Read a chunk of blocks
        size_t blocks_to_read = std::min(buffer_size, num_blocks - block_start);
        size_t ints_read = 0;
        
        for (size_t b = 0; b < blocks_to_read; b++) {
            int64_t block_buffer[ints_per_block];
            read_block(input, block_buffer, block_size, block_start + b);
            
            size_t block_ints = std::min(ints_per_block, size - (block_start + b) * ints_per_block);
            memcpy(buffer + ints_read, block_buffer, block_ints * sizeof(int64_t));
            ints_read += block_ints;
        }
        
        // Process the integers in the buffer
        for (size_t i = 0; i < ints_read; i++) {
            int64_t value = buffer[i];
            
            // Determine which partition this integer belongs to
            int partition = 0;
            while (partition < num_pivots && value > pivots[partition]) {
                partition++;
            }
            
            // Add the integer to the appropriate partition buffer
            partition_buffers[partition][partition_buffer_positions[partition]++] = value;
            partition_sizes[partition]++;
            
            // If the partition buffer is full, write it to disk
            if (partition_buffer_positions[partition] == ints_per_block) {
                write_block(partition_outputs[partition], partition_buffers[partition], 
                           block_size, partition_positions[partition]++);
                partition_buffer_positions[partition] = 0;
            }
        }
        
        block_start += blocks_to_read;
    }
    
    // Flush any remaining data in the partition buffers
    for (int i = 0; i < num_partitions; i++) {
        if (partition_buffer_positions[i] > 0) {
            // If the buffer is not full, pad with zeros
            if (partition_buffer_positions[i] < ints_per_block) {
                memset(partition_buffers[i] + partition_buffer_positions[i], 0, 
                      (ints_per_block - partition_buffer_positions[i]) * sizeof(int64_t));
            }
            
            write_block(partition_outputs[i], partition_buffers[i], 
                       block_size, partition_positions[i]);
        }
        
        // Close the partition file
        fclose(partition_outputs[i]);
        delete[] partition_buffers[i];
    }
    
    // Clean up resources
    delete[] buffer;
    delete[] partition_positions;
    delete[] partition_buffers;
    delete[] partition_buffer_positions;
    delete[] partition_outputs;
    
    fclose(input);
}