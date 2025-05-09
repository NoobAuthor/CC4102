#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "experiments.hpp"
#include "mergesort.hpp"
#include "utils.hpp"

// Constants
const size_t MB = 1024 * 1024;  // 1 MB in bytes
const size_t M = 50 * MB;       // M = 50 MB (max memory for sorting)
const size_t ELEMENT_SIZE = sizeof(int64_t);  // Size of a 64-bit integer

int main(int argc, char* argv[]) {
  // Seed the random number generator
  std::srand(std::time(nullptr));

  // Parse command line arguments
  std::string mode = "all";
  if (argc > 1) {
    mode = argv[1];
  }

  // Determine block size (based on actual disk block size)
  // For the purpose of this assignment, we'll use 4KB as a common block size
  size_t block_size = 4 * 1024;  // 4KB

  if (mode == "find_arity" || mode == "all") {
    std::cout << "Finding optimal arity..." << std::endl;

    // Generate a test file of 60M elements
    size_t test_size_mb = 60;
    size_t test_elements = test_size_mb * MB / ELEMENT_SIZE;
    std::string test_file = "test_60mb.bin";

    if (generate_random_array(test_file, test_elements, block_size)) {
      // Find the optimal arity
      size_t optimal_arity =
          find_optimal_arity(test_file, test_elements, block_size);
      std::cout << "Optimal arity: " << optimal_arity << std::endl;

      // Save the result to a file
      std::ofstream arity_file("optimal_arity.txt");
      arity_file << optimal_arity;
      arity_file.close();

      // Clean up
      std::remove(test_file.c_str());
    } else {
      std::cerr << "Failed to generate test file" << std::endl;
      return 1;
    }
  }

  if (mode == "experiment" || mode == "all") {
    // Get the optimal arity
    size_t arity = 0;
    std::ifstream arity_file("optimal_arity.txt");

    if (arity_file.is_open()) {
      arity_file >> arity;
      arity_file.close();
    } else {
      // Default to a reasonable value if not found
      arity = 10;
      std::cerr << "Could not find optimal_arity.txt, using default value: "
                << arity << std::endl;
    }

    // Define array sizes to test (in MB)
    std::vector<size_t> sizes = {4,  8,  12, 16, 20, 24, 28, 32,
                                 36, 40, 44, 48, 52, 56, 60};

    // Run experiments
    run_experiments(sizes, 5, block_size, arity);
  }

  return 0;
}
