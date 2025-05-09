#include "utils/test_generator.h"

#include <algorithm>
#include <random>

std::vector<int> generateRandomData(std::size_t size) {
  std::vector<int> data(size);

  // Create a random number generator
  std::random_device rd;
  std::mt19937 gen(rd());

  // Define the distribution (you can adjust the range as needed)
  std::uniform_int_distribution<int> distrib(0, 1000000);

  // Fill the vector with random numbers
  for (std::size_t i = 0; i < size; i++) {
    data[i] = distrib(gen);
  }

  return data;
}

std::vector<int64_t> generateRandomInt64Data(std::size_t size) {
  std::vector<int64_t> data(size);

  // Create a random number generator
  std::random_device rd;
  std::mt19937_64 gen(rd());

  // Define the distribution (using a wider range for int64_t)
  std::uniform_int_distribution<int64_t> distrib(0, 1000000000000LL);

  // Fill the vector with random numbers
  for (std::size_t i = 0; i < size; i++) {
    data[i] = distrib(gen);
  }

  return data;
}

std::vector<int> generateSortedData(std::size_t size) {
  std::vector<int> data(size);

  // Fill with sequential numbers
  for (std::size_t i = 0; i < size; i++) {
    data[i] = static_cast<int>(i);
  }

  return data;
}

std::vector<int> generateReverseSortedData(std::size_t size) {
  std::vector<int> data(size);

  // Fill with reverse sequential numbers
  for (std::size_t i = 0; i < size; i++) {
    data[i] = static_cast<int>(size - i - 1);
  }

  return data;
}

std::vector<int> generatePartiallySortedData(std::size_t size,
                                             double sortedFraction) {
  std::vector<int> data(size);

  // Fill with sequential numbers
  for (std::size_t i = 0; i < size; i++) {
    data[i] = static_cast<int>(i);
  }

  // Calculate how many elements to shuffle
  std::size_t shuffleCount =
      static_cast<std::size_t>(size * (1.0 - sortedFraction));

  // Create a random number generator
  std::random_device rd;
  std::mt19937 gen(rd());

  // Shuffle a portion of the array
  for (std::size_t i = 0; i < shuffleCount; i++) {
    std::uniform_int_distribution<std::size_t> distrib(0, size - 1);
    std::size_t pos1 = distrib(gen);
    std::size_t pos2 = distrib(gen);
    std::swap(data[pos1], data[pos2]);
  }

  return data;
}

// Add an alias for generateTestData to maintain compatibility with existing
// code
std::vector<int> generateTestData(std::size_t size) {
  return generateRandomData(size);
}