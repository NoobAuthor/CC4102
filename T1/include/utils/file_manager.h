#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief FileManager - A utility class for efficient filesystem operations
 *
 * This class centralizes file and directory operations, providing caching
 * and optimized methods for common tasks like finding files, checking
 * existence, and managing paths.
 */
class FileManager {
 public:
  /**
   * Get the singleton instance of FileManager
   * @return Reference to the FileManager instance
   */
  static FileManager& getInstance();

  /**
   * Check if a directory exists
   * @param path Directory path to check
   * @return True if exists, false otherwise
   */
  bool directoryExists(const std::string& path);

  /**
   * Create a directory if it doesn't exist
   * @param path Directory path to create
   */
  void createDirectoryIfNotExists(const std::string& path);

  /**
   * Check if a file exists (with caching for frequently checked files)
   * @param path File path to check
   * @return True if exists, false otherwise
   */
  bool fileExists(const std::string& path);

  /**
   * List files in a directory with optional filtering by pattern
   * @param directory Directory to list files from
   * @param pattern Optional pattern to filter by (e.g., "*.bin")
   * @return Vector of file paths
   */
  std::vector<std::string> listFiles(const std::string& directory,
                                     const std::string& pattern = "");

  /**
   * Find the largest available arity test file
   * @return Path to the largest arity test file or empty string if none found
   */
  std::string findLargestArityTestFile();

  /**
   * Find files matching a pattern and multiplier in the data/sequences
   * directory
   * @param multiplier Size multiplier to filter by (4, 8, 12, etc.)
   * @return Vector of matching file paths
   */
  std::vector<std::string> findSequenceFiles(size_t multiplier = 0);

  /**
   * Clear the file existence cache to force fresh checks
   */
  void clearCache();

 private:
  // Private constructor for singleton pattern
  FileManager();

  // Cache of directory existence checks
  std::unordered_map<std::string, bool> directoryCache;

  // Cache of file existence checks
  std::unordered_map<std::string, bool> fileCache;
};

#endif  // FILE_MANAGER_H