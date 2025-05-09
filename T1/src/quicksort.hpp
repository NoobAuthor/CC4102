#ifndef QUICKSORT_HPP
#define QUICKSORT_HPP

#include <string>

bool external_quicksort(const std::string& input_file,
                        const std::string& output_file, size_t n,
                        size_t block_size, size_t num_pivots);

#endif