#ifndef MERGESORT_HPP
#define MERGESORT_HPP

#include <string>

bool external_mergesort(const std::string& input_file,
                        const std::string& output_file, size_t n,
                        size_t block_size, size_t arity);
size_t find_optimal_arity(const std::string& test_file, size_t n,
                          size_t block_size);

#endif