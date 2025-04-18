#include "external_mergesort.hpp"
#include "utils.hpp"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>

void external_mergesort(const char* input_file, const char* output_file, 
                        size_t size, size_t block_size, 
                        size_t memory_limit, int arity) {
    size_t ints_per_block = integers_per_block(block_size);
    size_t max_ints_in_memory = memory_limit / sizeof(int64_t);
    size_t buffer_blocks = memory_limit / block_size / (arity + 1);
    
    int max_runs = (size + max_ints_in_memory - 1) / max_ints_in_memory;
    char** run_files = new char*[max_runs];
    for (int i = 0; i < max_runs; i++) {
        run_files[i] = new char[100];
        sprintf(run_files[i], "run_%d.bin", i);
    }
    
    int num_runs = create_initial_runs(input_file, run_files, 
                                       size, block_size, memory_limit);
    
    size_t* run_sizes = new size_t[num_runs];
    size_t remaining = size;
    for (int i = 0; i < num_runs; i++) {
        run_sizes[i] = std::min(max_ints_in_memory, remaining);
        remaining -= run_sizes[i];
    }
    
    char merge_output_file[100];
    int num_output_runs = 0;
    
    while (num_runs > 1) {
        int i;
        for (i = 0; i < num_runs; i += arity) {
            int num_to_merge = std::min(arity, num_runs - i);
            sprintf(merge_output_file, "merge_%d.bin", num_output_runs);
            
            merge_runs(run_files + i, run_sizes + i, 
                      merge_output_file, num_to_merge, 
                      block_size, buffer_blocks, arity);
            
            strcpy(run_files[num_output_runs], merge_output_file);
            run_sizes[num_output_runs] = 0;
            for (int j = 0; j < num_to_merge; j++) {
                run_sizes[num_output_runs] += run_sizes[i + j];
            }
            
            num_output_runs++;
        }
        
        num_runs = num_output_runs;
        num_output_runs = 0;
    }
    
    rename(run_files[0], output_file);
    
    for (int i = 0; i < max_runs; i++) {
        if (i >= num_runs) {
            remove(run_files[i]);
        }
        delete[] run_files[i];
    }
    delete[] run_files;
    delete[] run_sizes;
}

int create_initial_runs(const char* input_file, char** run_files,
                        size_t size, size_t block_size, 
                        size_t memory_limit) {
    FILE* input = fopen(input_file, "rb");
    if (!input) {
        std::cerr << "Could not open input file: " << input_file << std::endl;
        return 0;
    }
    
    size_t ints_per_block = integers_per_block(block_size);
    size_t max_ints_in_memory = memory_limit / sizeof(int64_t);
    size_t blocks_in_memory = max_ints_in_memory / ints_per_block;
    
    int64_t* buffer = new int64_t[max_ints_in_memory];
    
    int num_runs = 0;
    size_t remaining = size;
    
    while (remaining > 0) {
        size_t run_ints = std::min(max_ints_in_memory, remaining);
        size_t run_blocks = (run_ints + ints_per_block - 1) / ints_per_block;
        
        size_t ints_read = 0;
        for (size_t b = 0; b < run_blocks && ints_read < run_ints; b++) {
            int64_t block_buffer[ints_per_block];
            read_block(input, block_buffer, block_size, num_runs * blocks_in_memory + b);
            
            size_t block_ints = std::min(ints_per_block, run_ints - ints_read);
            memcpy(buffer + ints_read, block_buffer, block_ints * sizeof(int64_t));
            ints_read += block_ints;
        }
        
        std::sort(buffer, buffer + run_ints);
        
        FILE* run_file = fopen(run_files[num_runs], "wb");
        if (!run_file) {
            std::cerr << "Could not create run file: " << run_files[num_runs] << std::endl;
            delete[] buffer;
            fclose(input);
            return num_runs;
        }
        
        size_t ints_written = 0;
        for (size_t b = 0; b < run_blocks && ints_written < run_ints; b++) {
            size_t block_ints = std::min(ints_per_block, run_ints - ints_written);
            int64_t block_buffer[ints_per_block];
            
            memcpy(block_buffer, buffer + ints_written, block_ints * sizeof(int64_t));
            
            if (block_ints < ints_per_block) {
                memset(block_buffer + block_ints, 0, (ints_per_block - block_ints) * sizeof(int64_t));
            }
            
            write_block(run_file, block_buffer, block_size, b);
            ints_written += block_ints;
        }
        
        fclose(run_file);
        num_runs++;
        remaining -= run_ints;
    }
    
    delete[] buffer;
    fclose(input);
    
    return num_runs;
}

void merge_runs(char** run_files, size_t* run_sizes, 
                const char* output_file, int num_runs,
                size_t block_size, size_t buffer_blocks, 
                int arity) {
    if (num_runs <= 0) return;
    if (num_runs == 1) {
        rename(run_files[0], output_file);
        return;
    }
    
    FILE** input_files = new FILE*[num_runs];
    for (int i = 0; i < num_runs; i++) {
        input_files[i] = fopen(run_files[i], "rb");
        if (!input_files[i]) {
            std::cerr << "Could not open run file: " << run_files[i] << std::endl;
            for (int j = 0; j < i; j++) {
                fclose(input_files[j]);
            }
            delete[] input_files;
            return;
        }
    }
    
    FILE* output = fopen(output_file, "wb");
    if (!output) {
        std::cerr << "Could not create output file: " << output_file << std::endl;
        for (int i = 0; i < num_runs; i++) {
            fclose(input_files[i]);
        }
        delete[] input_files;
        return;
    }
    
    size_t ints_per_block = integers_per_block(block_size);
    size_t input_buffer_blocks = buffer_blocks / (num_runs + 1);
    size_t output_buffer_blocks = buffer_blocks - (input_buffer_blocks * num_runs);
    
    if (input_buffer_blocks == 0) input_buffer_blocks = 1;
    if (output_buffer_blocks == 0) output_buffer_blocks = 1;
    
    int64_t** input_buffers = new int64_t*[num_runs];
    for (int i = 0; i < num_runs; i++) {
        input_buffers[i] = new int64_t[input_buffer_blocks * ints_per_block];
    }
    int64_t* output_buffer = new int64_t[output_buffer_blocks * ints_per_block];
    
    size_t* current_positions = new size_t[num_runs];
    size_t* buffer_positions = new size_t[num_runs];
    size_t* buffer_sizes = new size_t[num_runs];
    size_t* remaining_elements = new size_t[num_runs];
    
    for (int i = 0; i < num_runs; i++) {
        current_positions[i] = 0;
        buffer_positions[i] = 0;
        buffer_sizes[i] = 0;
        remaining_elements[i] = run_sizes[i];
    }
    
    for (int i = 0; i < num_runs; i++) {
        if (remaining_elements[i] > 0) {
            size_t blocks_to_read = std::min(input_buffer_blocks, 
                                         (remaining_elements[i] + ints_per_block - 1) / ints_per_block);
            for (size_t b = 0; b < blocks_to_read; b++) {
                read_block(input_files[i], input_buffers[i] + b * ints_per_block, 
                          block_size, current_positions[i]++);
            }
            
            buffer_sizes[i] = std::min(blocks_to_read * ints_per_block, remaining_elements[i]);
            remaining_elements[i] -= buffer_sizes[i];
        }
    }
    
    size_t output_position = 0;
    size_t output_buffer_position = 0;
    
    bool runs_remaining = true;
    while (runs_remaining) {
        int smallest_run = -1;
        for (int i = 0; i < num_runs; i++) {
            if (buffer_positions[i] < buffer_sizes[i]) {
                if (smallest_run == -1 || 
                    input_buffers[i][buffer_positions[i]] < 
                    input_buffers[smallest_run][buffer_positions[smallest_run]]) {
                    smallest_run = i;
                }
            }
        }
        
        if (smallest_run == -1) {
            runs_remaining = false;
            break;
        }
        
        output_buffer[output_buffer_position++] = 
            input_buffers[smallest_run][buffer_positions[smallest_run]++];
        
        if (output_buffer_position == output_buffer_blocks * ints_per_block) {
            write_block(output, output_buffer, block_size, output_position++);
            output_buffer_position = 0;
        }
        
        if (buffer_positions[smallest_run] == buffer_sizes[smallest_run] && 
            remaining_elements[smallest_run] > 0) {
            
            size_t blocks_to_read = std::min(input_buffer_blocks, 
                                         (remaining_elements[smallest_run] + ints_per_block - 1) / 
                                         ints_per_block);
            
            for (size_t b = 0; b < blocks_to_read; b++) {
                read_block(input_files[smallest_run], 
                          input_buffers[smallest_run] + b * ints_per_block, 
                          block_size, current_positions[smallest_run]++);
            }
            
            buffer_positions[smallest_run] = 0;
            buffer_sizes[smallest_run] = std::min(blocks_to_read * ints_per_block, 
                                              remaining_elements[smallest_run]);
            remaining_elements[smallest_run] -= buffer_sizes[smallest_run];
        }
    }
    
    if (output_buffer_position > 0) {
        if (output_buffer_position < output_buffer_blocks * ints_per_block) {
            memset(output_buffer + output_buffer_position, 0, 
                  (output_buffer_blocks * ints_per_block - output_buffer_position) * sizeof(int64_t));
        }
        
        write_block(output, output_buffer, block_size, output_position);
    }
    
    for (int i = 0; i < num_runs; i++) {
        fclose(input_files[i]);
        remove(run_files[i]);  
        delete[] input_buffers[i];
    }
    delete[] input_buffers;
    delete[] output_buffer;
    delete[] input_files;
    delete[] current_positions;
    delete[] buffer_positions;
    delete[] buffer_sizes;
    delete[] remaining_elements;
    
    fclose(output);
}

int64_t test_mergesort(const char* filename, size_t size, 
                      size_t block_size, size_t memory_limit, 
                      int arity) {
    char test_input[100];
    sprintf(test_input, "%s.test", filename);
    
    FILE* src = fopen(filename, "rb");
    FILE* dst = fopen(test_input, "wb");
    
    if (!src || !dst) {
        std::cerr << "Could not open files for test copy" << std::endl;
        if (src) fclose(src);
        if (dst) fclose(dst);
        return INT64_MAX;
    }
    
    int64_t buffer[integers_per_block(block_size)];
    size_t blocks = (size + integers_per_block(block_size) - 1) / integers_per_block(block_size);
    
    for (size_t i = 0; i < blocks; i++) {
        read_block(src, buffer, block_size, i);
        write_block(dst, buffer, block_size, i);
    }
    
    fclose(src);
    fclose(dst);
    
    char test_output[100];
    sprintf(test_output, "%s.sorted", filename);
    
    reset_disk_access_counter();
    
    double time = measure_execution_time([&]() {
        external_mergesort(test_input, test_output, size, block_size, memory_limit, arity);
    });
    
    int64_t io_count = get_disk_access_count();
    
    bool sorted = is_sorted(test_output, size, block_size);
    if (!sorted) {
        std::cerr << "Warning: Result is not sorted for arity " << arity << std::endl;
        return INT64_MAX;
    }
    
    remove(test_input);
    remove(test_output);
    
    return io_count + static_cast<int64_t>(time * 1000);
}