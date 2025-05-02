#include "external_quicksort.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>
#include <random>
#include <ctime>

/**
 * TODO:
 *  - Implement external_quicksort function [x]
 * - Implement external_quicksort_recursive function [x]
 * - Implement select_pivots function []
 * - Implement partition_file function []
 */

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
        
        int64_t* buffer = new int64_t[size]; // Allocate memory for the entire file
        size_t blocks_to_read = (size + ints_per_block - 1) / ints_per_block; // Calculate number of blocks to read
        
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
		// Why would we need to partition? 
		// Because the data is too large to fit in memory at once.
    int64_t* pivots = new int64_t[num_pivots];
    select_pivots(input_file, size, block_size, num_pivots, pivots);
    
    // Number of partitions is one more than the number of pivots
    int num_partitions = num_pivots + 1;
    
    // Create temporary files for each partition
    char** partition_files = new char*[num_partitions];
    for (int i = 0; i < num_partitions; i++) {
        partition_files[i] = new char[100];
        sprintf(partition_files[i], "partition_%d_%d.bin", depth, i);  // Unique name for each partition
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
		// Why do we need to concatenate the sorted partitions?
		// Because the final output needs to be a single sorted file.
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
    
    size_t output_position = 0; // Position in the output file
    int64_t buffer[ints_per_block]; // Buffer for reading blocks from partitions 
    
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
            } // Read and write each block from the partition
            
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

