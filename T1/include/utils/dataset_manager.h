#ifndef DATASET_MANAGER_H
#define DATASET_MANAGER_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

/**
 * @brief The DatasetManager class is responsible for generating and managing
 * datasets
 */
class DatasetManager {
 public:
  explicit DatasetManager(unsigned int seed = 0);

  /**
   * @brief Generates a dataset of integers based on the specified parameters.
   * @param size The size of the dataset to generate.
   * @param type The type of dataset to generate. Options include "random",
   * "sorted", "reverse_sorted", and "partially_sorted".
   * @param sortedFraction The fraction of the dataset that should be sorted
   * (only applicable for "partially_sorted").
   */
  std::vector<int> generateIntData(std::size_t size,
                                   const std::string& type = "random",
                                   double sortedFraction = 1.0);

  /**
   * @brief Generates a dataset of 64-bit integers based on the specified
   * parameters.
   * @param size The size of the dataset to generate.
   * @param type The type of dataset to generate. Options include "random",
   * "sorted", "reverse_sorted", and "partially_sorted".
   * @param sortedFraction The fraction of the dataset that should be sorted
   * (only applicable for "partially_sorted").
   */
  std::vector<int64_t> generateInt64Data(std::size_t size,
                                         const std::string& type = "random",
                                         double sortedFraction = 1.0);

  /**
   * @brief Generates a dataset of integers based on the specified parameters
   * @param size The size of the dataset to generate.
   * @param filename The name of the file to save the dataset to.
   */
  void generateAndSaveInt64Sequence(std::size_t size,
                                    const std::string& filename);

  /**
   * @brief Generates experiment datasets for
   * benchmarking purposes.
   * @param maxMultiplier The maximum multiplier
   */
  void generateExperimentDatasets(std::size_t maxMultiplier = 60);

  /**
   * @brief Check if all datasets exist.
   * @param maxMultiplier The maximum multiplier
   */
  bool checkAllDatasetsExist(std::size_t maxMultiplier = 60);

  static std::vector<int> generateRandomData(std::size_t size);
  static std::vector<int64_t> generateRandomInt64Data(std::size_t size);
  static std::vector<int> generateSortedData(std::size_t size);
  static std::vector<int> generateReverseSortedData(std::size_t size);
  static std::vector<int> generatePartiallySortedData(std::size_t size,
                                                      double sortedFraction);
  static std::vector<int> generateRandomData(std::size_t size);
  static std::vector<int> generateTestData(std::size_t size);

 private:
  unsigned int _seed;
  bool _useRandomSeed;
};

#endif