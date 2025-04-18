#ifndef EXTERNAL_MERGESORT_HPP
#define EXTERNAL_MERGESORT_HPP

#include <cstddef>
#include <cstdint>

/**
 * Sort a file using External Mergesort algorithm.
 * @param input_file Path to input file 
 * @param output_file Path to output file
 * @param size Number of intefers in the file
 * @param block_size Size of disk blocks in bytes 
 * @param memory_limit Memory limit in bytes
 * @param arity Number of runs to merge at once
 */
void external_mergesort(const char* input_file, const char* output_file, size_t size, size_t block_size, size_t memory_limit, int arity);

/**
 * Create initial sorted runs
 * @param input_file Path to input file
 * @param run_files Array of run file paths
 * @param size Number of integers in the file
 * @param block_size Size of disk blocks in bytes
 * @param memory_limit Memory limit in bytes
 * @return Number of runs created
 */
int create_initial_runs(const char* input_file, const char** run_files, size_t size, size_t block_size, size_t memory_limit);

/**
 * Merge a group of runs
 * @param run_files Array of run file paths
 * @param run_sizes Array of run sizes
 * @param output_file Path to output file
 * @param num_runs Number of runs to merge
 * @param block_size Size of disk blocks in bytes
 * @param buffer_blocks Number of blocks for buffer
 * @param arity Number of runs to merge at once
 */
void merge_runs(const char** run_files, size_t* run_sizes, const char* output_file, int num_runs, size_t block_size, size_t buffer_blocks, int arity);

/**
 * Test Mergesort with given parameters and return performance metrics
 * @param filename Path to input file
 * @param size Number of integers in the file
 * @param block_size Size of disk blocks in bytes
 * @param memory_limit Memory limit in bytes
 * @param arity Number of runs to merge at once
 * @return Performance metrics (weighted combination of time and I/O)
 */
int64_t test_mergesort(const char* filename, size_t size, size_t block_size, size_t memory_limit, int arity);

#endif // EXTERNAL_MERGESORT_HPP 