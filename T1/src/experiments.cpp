#include "experiments.hpp"

#include <chrono>
#include <iostream>
#include <string>

#include "mergesort.hpp"
#include "quicksort.hpp"
#include "utils.hpp"

// Constants
const size_t MB = 1024 * 1024;  // 1 MB in bytes
const size_t M = 50 * MB;       // M = 50 MB (max memory for sorting)
const size_t ELEMENT_SIZE = sizeof(int64_t);  // Size of a 64-bit integer

void run_experiments(const std::vector<std::size_t>& sizes,
                     std::size_t num_trials, std::size_t block_size,
                     std::size_t arity) {
  std::cout << "Running experiments with arity = " << arity << std::endl;
  std::cout << "Size(MB),MergeSort_Time(ms),MergeSort_IO,QuickSort_Time(ms),"
               "QuickSort_IO"
            << std::endl;

  for (size_t size_mb : sizes) {
    size_t n = size_mb * MB / ELEMENT_SIZE;  // Convert MB to number of elements

    std::vector<double> merge_times;
    std::vector<size_t> merge_ios;
    std::vector<double> quick_times;
    std::vector<size_t> quick_ios;

    for (size_t trial = 0; trial < num_trials; ++trial) {
      std::string input_file = "data_" + std::to_string(size_mb) + "mb_" +
                               std::to_string(trial) + ".bin";

      // Generate random data
      generate_random_array(input_file, n, block_size);

      // Test Mergesort
      {
        std::string merge_output = "merge_sorted_" + std::to_string(size_mb) +
                                   "mb_" + std::to_string(trial) + ".bin";

        disk_reads = 0;
        disk_writes = 0;

        auto start = std::chrono::high_resolution_clock::now();
        external_mergesort(input_file, merge_output, n, block_size, arity);
        auto end = std::chrono::high_resolution_clock::now();

        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        merge_times.push_back(duration.count());
        merge_ios.push_back(disk_reads + disk_writes);

        // Verify that the output is sorted
        if (!check_sorted(merge_output, n, block_size)) {
          std::cerr << "Error: Mergesort output is not sorted!" << std::endl;
        }

        // Clean up
        std::remove(merge_output.c_str());
      }

      // Test Quicksort
      {
        std::string quick_output = "quick_sorted_" + std::to_string(size_mb) +
                                   "mb_" + std::to_string(trial) + ".bin";

        disk_reads = 0;
        disk_writes = 0;

        auto start = std::chrono::high_resolution_clock::now();
        external_quicksort(input_file, quick_output, n, block_size,
                           arity - 1);  // arity-1 pivots
        auto end = std::chrono::high_resolution_clock::now();

        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        quick_times.push_back(duration.count());
        quick_ios.push_back(disk_reads + disk_writes);

        // Verify that the output is sorted
        if (!check_sorted(quick_output, n, block_size)) {
          std::cerr << "Error: Quicksort output is not sorted!" << std::endl;
        }

        // Clean up
        std::remove(quick_output.c_str());
      }

      // Clean up input file
      std::remove(input_file.c_str());
    }

    // Calculate averages
    double avg_merge_time = 0.0;
    size_t avg_merge_io = 0;
    double avg_quick_time = 0.0;
    size_t avg_quick_io = 0;

    for (size_t i = 0; i < num_trials; ++i) {
      avg_merge_time += merge_times[i];
      avg_merge_io += merge_ios[i];
      avg_quick_time += quick_times[i];
      avg_quick_io += quick_ios[i];
    }

    avg_merge_time /= num_trials;
    avg_merge_io /= num_trials;
    avg_quick_time /= num_trials;
    avg_quick_io /= num_trials;

    // Output results
    std::cout << size_mb << "," << avg_merge_time << "," << avg_merge_io << ","
              << avg_quick_time << "," << avg_quick_io << std::endl;
  }
}