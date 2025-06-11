#!/bin/bash

# Create necessary directories
mkdir -p input results

# Compile the C++ code
echo "Compiling C++ code..."
make clean
make

# Generate input data
echo "Generating input data..."
python3 graph/generate_points.py

# Run experiments
echo "Running experiments..."
for n in {5..12}; do
    size=$((2**n))
    echo "Testing n = $size..."
    for run in {0..4}; do
        input_file="input/input_n${size}_run${run}.txt"
        ./kruskal "$input_file"
    done
done

# Plot results
echo "Plotting results..."
python3 graph/plot_results.py

echo "Experiment completed! Results are in results/timing.csv and results/performance_plot.png" 