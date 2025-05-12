#include "utils/test_generator.h"

#include <algorithm>
#include <random>

std::vector<int> generateRandomData(std::size_t size) {
  std::vector<int> data(size);

  std::random_device rd;
  std::mt19937 gen(rd());

  std::uniform_int_distribution<int> distrib(0, 1000000);

  for (std::size_t i = 0; i < size; i++) {
    data[i] = distrib(gen);
  }

  return data;
}

std::vector<int64_t> generateRandomInt64Data(std::size_t size) {
  std::vector<int64_t> data(size);

  std::random_device rd;
  std::mt19937_64 gen(rd());

  std::uniform_int_distribution<int64_t> distrib(0, 1000000000000LL);

  for (std::size_t i = 0; i < size; i++) {
    data[i] = distrib(gen);
  }

  return data;
}

std::vector<int> generateSortedData(std::size_t size) {
  std::vector<int> data(size);

  for (std::size_t i = 0; i < size; i++) {
    data[i] = static_cast<int>(i);
  }

  return data;
}

std::vector<int> generateReverseSortedData(std::size_t size) {
  std::vector<int> data(size);

  for (std::size_t i = 0; i < size; i++) {
    data[i] = static_cast<int>(size - i - 1);
  }

  return data;
}

std::vector<int> generatePartiallySortedData(std::size_t size,
                                             double sortedFraction) {
  std::vector<int> data(size);

  for (std::size_t i = 0; i < size; i++) {
    data[i] = static_cast<int>(i);
  }

  std::size_t elementsToShuffle =
      static_cast<std::size_t>(size * (1.0 - sortedFraction));

  if (elementsToShuffle > 0) {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::shuffle(data.begin(), data.begin() + elementsToShuffle, gen);
  }

  return data;
}