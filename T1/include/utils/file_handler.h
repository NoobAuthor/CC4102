#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief Reads a file and returns its contents as a vector of integers.
 * @param filename The name of the file to read.
 */
std::vector<int> readFromFile(const std::string& filename);

/**
 * @brief Writes a vector of integers to a file.
 * @param filename The name of the file to write to.
 * @param data The vector of integers to write.
 */
void writeToFile(const std::string& filename, const std::vector<int>& data);

/**
 * @brief Reads a file and returns its contents as a vector of 64-bit integers.
 * @param filename The name of the file to read.
 */
std::vector<int> readDataFromFile(const std::string& filename);

/**
 * @brief Writes a vector of 64-bit integers to a file.
 * @param filename The name of the file to write to.
 * @param data The vector of 64-bit integers to write.
 */
void writeDataToFile(const std::string& filename, const std::vector<int>& data);

/**
 * @brief Reads a file and returns its contents as a vector of 64-bit integers.
 * @param filename The name of the file to read.
 */
std::vector<int64_t> readInt64FromFile(const std::string& filename);

/**
 * @brief Writes a vector of 64-bit integers to a file.
 * @param filename The name of the file to write to.
 * @param data The vector of 64-bit integers to write.
 */
void writeInt64ToFile(const std::string& filename,
                      const std::vector<int64_t>& data);

/**
 * @brief Reads a file and returns its contents as a vector of 64-bit integers.
 * @param filename The name of the file to read.
 */
std::vector<int64_t> readInt64DataFromFile(const std::string& filename);

/**
 * @brief Writes a vector of 64-bit integers to a file.
 * @param filename The name of the file to write to.
 * @param data The vector of 64-bit integers to write.
 */
void writeInt64DataToFile(const std::vector<int64_t>& data,
                          const std::string& filename);

/**
 * @brief Reads a block of data from a file.
 * @param filename The name of the file to read from.
 * @param offset The offset in the file to start reading from.
 * @param blockSize The size of the block to read.
 */
std::vector<int64_t> readBlockFromFile(const std::string& filename,
                                       size_t offset, size_t blockSize);
/**
 * @brief Writes a block of data to a file.
 * @param filename The name of the file to write to.
 * @param block The block of data to write.
 * @param offset The offset in the file to start writing to.
 */
void writeBlockToFile(const std::string& filename,
                      const std::vector<int64_t>& block, size_t offset);

/**
 * @brief Appends a block of data to a file.
 * @param filename The name of the file to append to.
 */
void appendInt64DataToFile(const std::vector<int64_t>& data,
                           const std::string& filename);

/**
 * @brief Generates a dataset of integers based on the specified parameters.
 * @param size The size of the dataset to generate.
 */
static std::vector<int> generateTestData(std::size_t size);

extern size_t disk_read_count;
extern size_t disk_write_count;
void resetDiskCounters();
size_t getDiskReadCount();
size_t getDiskWriteCount();

#endif  // FILE_HANDLER_H