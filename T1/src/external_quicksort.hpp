#ifndef EXTERNAL_QUICKSORT_HPP
#define EXTERNAL_QUICKSORT_HPP

#include <cstddef>
#include <cstdint>

/**
 * Sort a file using External Quicksort algorithm-
 * @param input_file The path to the input file.
 * @param output_file The path to the output file.
 * @param size Number of integers in the file.
 * @param memory_limit The maximum amount of memory to use for sorting.
 * @param num_pivots Number of pivots to use (arity - 1).
 */
void external_quicksort(const char* input_file, const char* output_file, size_t size, size_t block_size, size_t memory_limit, int num_pivots);

/**
 * Sort a file using External Quicksort algorithm (recursive helper).
 * @param input_file The path to the input file.
 * @param output_file The path to the output file.
 * @param size Number of integers in the file.
 * @param block_size The size of the blocks to read from the file.
 * @param memory_limit The maximum amount of memory to use for sorting.
 * @param num_pivots Number of pivots to use (arity - 1).
 * @param depth The current depth of the recursion.
 */
void external_quicksort_recursive(const char* input_file, const char* output_file, size_t size, size_t block_size, size_t memory_limit, int num_pivots, int depth);

/**
 * Select pivots from the input file.
 * @param input_file The path to the input file.
 * @param size Number of integers in the file.
 * @param block_size The size of the blocks to read from the file.
 * @param num_pivots Number of pivots to select.
 * @param pivots Array to store the selected pivots.
 */
void select_pivots(const char* input_file, size_t size, size_t block_size, int num_pivots, int64_t* pivots);

/**
 * Partition the input file based on the selected pivots.
 * @param input_file The path to the input file.
 * @param partition_files Array of partition file paths.
 * @param partition_sizes Array to store the sizes of each partition.
 * @param size Number of integers in the file.
 * @param block_size The size of the blocks to read from the file.
 * @param memmory_limit The maximum amount of memory to use for sorting.
 * @param pivot| Array of pivots.
 * @param num_pivots Number of pivots to use (arity - 1).
 */
void partition_file(const char* input_file, const char** partition_files, size_t* partition_sizes, size_t size, size_t block_size, size_t memory_limit, int64_t* pivots, int num_pivots);

#endif // EXTERNAL_QUICKSORT_HPP