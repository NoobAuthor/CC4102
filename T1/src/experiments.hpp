#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include <cstddef>  // Add this
#include <vector>

void run_experiments(const std::vector<std::size_t>& sizes,
                     std::size_t num_trials, std::size_t block_size,
                     std::size_t arity);

#endif