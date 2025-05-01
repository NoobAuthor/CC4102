#ifndef UTILS_HPP
#define UTILS_HPP

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <functional>

// Global counter for disk accesses.
extern int64_t g_disk_access_count;

/**
 * Write a block of data to a file at the specified position.
 * @param file File pointer to write to.
 * @param buffer Data buffer to write from.
 * @param block_size Size of the block in bytes.
 * @param position Position in the file in blocks.
 */
void write_block(FILE* file, int64_t* buffer, size_t block_size, long position);

/**
 * Read a block of data from a file at the specified position.
 * @param file File pointer to read from.
 * @param buffer Buffer to store the read data.
 * @param block_size Size of the block in bytes.
 * @param position Position in the file in blocks.
 */
void read_block(FILE* file, int64_t* buffer, size_t block_size, long position);

/**
 * Generate a binary file with random 64-bit integers.
 * @param filename Name of the file to create.
 * @param size Number of integers to generate.
 * @param block_size Size of each block in bytes.
 */
void generate_random_array(const char* filename, size_t size, size_t block_size);

/**
 * Check if a binary file contains sorted integers.
 * @param filename Name of the file to check.
 * @param size Number of integers to check.
 * @param block_size Size of each block in bytes.
 * @return true if sorted, false otherwise.
 */
bool is_sorted(const char* filename, size_t size, size_t block_size);

/**
 * Reset the disk access counter
 */
void reset_disk_access_counter();

/**
 * Get the current disk acces count
 * @return Number of disk accesses since last reset
 */
int64_t get_disk_access_count();

/**
 * Measure execution time of a function
 * @param algorithm Function to measure
 * @param return Execution time in seconds
 */
double measure_execution_time(std::function<void()> algorithm);

/**
 * Get the number of 64-bit integers that fit in a block
 * @param block_size Size of a block in bytes
 * @return Number of integers per block
 */
size_t integers_per_block(size_t block_size);

#endif  // UTILS_HPP
