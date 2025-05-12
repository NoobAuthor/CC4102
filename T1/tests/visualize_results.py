#!/usr/bin/env python3

try:
    import pandas as pd
    import matplotlib.pyplot as plt
    import numpy as np
    import os
    import glob
except ImportError as e:
    print(f"Error importing required libraries: {e}")
    print("Please install the required packages with: pip install pandas matplotlib numpy")
    exit(1)

# Create results directory if it doesn't exist
os.makedirs('results', exist_ok=True)

# Check if required data files exist
required_files = [
    'data/results/mergesort_results.csv',
    'data/results/quicksort_results.csv',
]

for file in required_files:
    if not os.path.exists(file):
        print(f"Warning: {file} not found. Some visualizations may be incomplete.")

print("Generating comprehensive visualizations for all experiments...")

# Process and average the results
def process_algorithm_results(file_path):
    try:
        # Read the data
        df = pd.read_csv(file_path)
        
        # Group by multiplier and calculate averages across the 5 sequences
        avg_results = df.groupby('Multiplier').agg({
            'Size': 'first',  # Keep the size
            'Time': 'mean',   # Average time
            'DiskReads': 'mean',  # Average disk reads
            'DiskWrites': 'mean'  # Average disk writes
        }).reset_index()
        
        return avg_results
    except Exception as e:
        print(f"Error processing {file_path}: {e}")
        return None

# Create execution time comparison graph
def create_time_comparison_graph():
    try:
        # Process both algorithm results
        merge_results = process_algorithm_results('data/results/mergesort_results.csv')
        quick_results = process_algorithm_results('data/results/quicksort_results.csv')
        
        if merge_results is None or quick_results is None:
            print("Cannot create time comparison graph due to missing data")
            return
        
        plt.figure(figsize=(12, 8))
        
        # Plot both algorithms
        plt.plot(merge_results['Multiplier'], merge_results['Time'], 
                 'b-o', label='MergeSort')
        plt.plot(quick_results['Multiplier'], quick_results['Time'], 
                 'r-o', label='QuickSort')
        
        plt.title('Average Execution Time vs Array Size')
        plt.xlabel('Array Size (multiples of M, where M=50MB)')
        plt.ylabel('Execution Time (seconds)')
        plt.grid(True)
        plt.legend()
        
        plt.savefig('results/time_comparison.png')
        plt.close()
        print("Created time comparison graph")
        
    except Exception as e:
        print(f"Error creating time comparison graph: {e}")

# Create disk operations comparison graph
def create_disk_operations_graph():
    try:
        # Process both algorithm results
        merge_results = process_algorithm_results('data/results/mergesort_results.csv')
        quick_results = process_algorithm_results('data/results/quicksort_results.csv')
        
        if merge_results is None or quick_results is None:
            print("Cannot create disk operations graph due to missing data")
            return
        
        plt.figure(figsize=(12, 8))
        
        # Calculate total disk operations
        merge_results['TotalDiskOps'] = merge_results['DiskReads'] + merge_results['DiskWrites']
        quick_results['TotalDiskOps'] = quick_results['DiskReads'] + quick_results['DiskWrites']
        
        # Plot total disk operations
        plt.plot(merge_results['Multiplier'], merge_results['TotalDiskOps'], 
                'b-o', label='MergeSort')
        plt.plot(quick_results['Multiplier'], quick_results['TotalDiskOps'], 
                'r-o', label='QuickSort')
        
        # Add this line to set logarithmic scale
        plt.yscale('log')
        
        plt.title('Average Disk Operations vs Array Size (Log Scale)')
        plt.xlabel('Array Size (multiples of M, where M=50MB)')
        plt.ylabel('Number of Disk Operations (log scale)')
        plt.grid(True, which="both", ls="-")
        plt.legend()
        
        plt.savefig('results/disk_operations_comparison.png')
        plt.close()
        print("Created disk operations comparison graph")
        
    except Exception as e:
        print(f"Error creating disk operations graph: {e}")

# Create detailed disk operations graph (reads vs writes)
def create_detailed_disk_operations_graph():
    try:
        # Process both algorithm results
        merge_results = process_algorithm_results('data/results/mergesort_results.csv')
        quick_results = process_algorithm_results('data/results/quicksort_results.csv')
        
        if merge_results is None or quick_results is None:
            print("Cannot create detailed disk operations graph due to missing data")
            return
        
        plt.figure(figsize=(12, 8))
        
        # Plot reads and writes separately
        plt.plot(merge_results['Multiplier'], merge_results['DiskReads'], 
                 'b-o', label='MergeSort Reads')
        plt.plot(merge_results['Multiplier'], merge_results['DiskWrites'], 
                 'b--o', label='MergeSort Writes')
        plt.plot(quick_results['Multiplier'], quick_results['DiskReads'], 
                 'r-o', label='QuickSort Reads')
        plt.plot(quick_results['Multiplier'], quick_results['DiskWrites'], 
                 'r--o', label='QuickSort Writes')
        
        # Add logarithmic scale
        plt.yscale('log')
        
        plt.title('Average Disk Reads and Writes vs Array Size (Log Scale)')
        plt.xlabel('Array Size (multiples of M, where M=50MB)')
        plt.ylabel('Number of Disk Operations (log scale)')
        plt.grid(True, which="both", ls="-")
        plt.legend()
        
        plt.savefig('results/detailed_disk_operations.png')
        plt.close()
        print("Created detailed disk operations graph")
        
    except Exception as e:
        print(f"Error creating detailed disk operations graph: {e}")

# Create time efficiency ratio graph
def create_efficiency_ratio_graph():
    try:
        # Process both algorithm results
        merge_results = process_algorithm_results('data/results/mergesort_results.csv')
        quick_results = process_algorithm_results('data/results/quicksort_results.csv')
        
        if merge_results is None or quick_results is None:
            print("Cannot create efficiency ratio graph due to missing data")
            return
        
        # Ensure we have the same multipliers in both results
        common_multipliers = set(merge_results['Multiplier']).intersection(
            set(quick_results['Multiplier']))
        
        if not common_multipliers:
            print("No common data points between algorithms")
            return
        
        merge_filtered = merge_results[merge_results['Multiplier'].isin(common_multipliers)]
        quick_filtered = quick_results[quick_results['Multiplier'].isin(common_multipliers)]
        
        # Sort by multiplier to ensure alignment
        merge_filtered = merge_filtered.sort_values('Multiplier')
        quick_filtered = quick_filtered.sort_values('Multiplier')
        
        plt.figure(figsize=(12, 8))
        
        # Calculate speedup ratio (MergeSort time / QuickSort time)
        speedup = merge_filtered['Time'].values / quick_filtered['Time'].values
        
        plt.plot(merge_filtered['Multiplier'], speedup, 'g-o')
        
        plt.axhline(y=1, color='r', linestyle='--', label='Equal performance')
        
        plt.title('Algorithm Speedup Ratio (MergeSort time / QuickSort time)')
        plt.xlabel('Array Size (multiples of M, where M=50MB)')
        plt.ylabel('Speedup Ratio')
        plt.grid(True)
        plt.legend()
        
        plt.savefig('results/speedup_ratio.png')
        plt.close()
        print("Created efficiency ratio graph")
        
    except Exception as e:
        print(f"Error creating efficiency ratio graph: {e}")

# Run all visualization functions
if __name__ == '__main__':
    try:
        # Create all visualizations
        create_time_comparison_graph()
        create_disk_operations_graph()
        create_detailed_disk_operations_graph()
        create_efficiency_ratio_graph()
        
        print("\nVisualization complete. Results saved in the 'results' directory.")
    except Exception as e:
        print(f"Error in visualization process: {e}")
        import traceback
        traceback.print_exc()