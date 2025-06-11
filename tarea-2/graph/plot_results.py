#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np
from pathlib import Path

def analyze_results(df):
    """Analyze the results and print insights."""
    print("\nPerformance Analysis:")
    print("-" * 50)
    
    # Calculate average speedup from path compression
    df_mean = df.groupby(['n', 'variant'])['time'].mean().reset_index()
    
    # Analyze speedup for array-based implementation
    array_data = df_mean[df_mean['variant'].isin(['PC+Array', 'NoPC+Array'])]
    array_pivot = array_data.pivot(index='n', columns='variant', values='time')
    array_speedup = array_pivot['NoPC+Array'] / array_pivot['PC+Array']
    
    # Analyze speedup for heap-based implementation
    heap_data = df_mean[df_mean['variant'].isin(['PC+Heap', 'NoPC+Heap'])]
    heap_pivot = heap_data.pivot(index='n', columns='variant', values='time')
    heap_speedup = heap_pivot['NoPC+Heap'] / heap_pivot['PC+Heap']
    
    print("\n1. Path Compression Speedup:")
    print(f"   Array-based: {array_speedup.mean():.2f}x average speedup")
    print(f"   Heap-based:  {heap_speedup.mean():.2f}x average speedup")
    
    # Analyze data structure impact
    pc_data = df_mean[df_mean['variant'].isin(['PC+Array', 'PC+Heap'])]
    pc_pivot = pc_data.pivot(index='n', columns='variant', values='time')
    ds_speedup = pc_pivot['PC+Array'] / pc_pivot['PC+Heap']
    
    print("\n2. Data Structure Impact (Array vs Heap):")
    print(f"   With Path Compression: {ds_speedup.mean():.2f}x difference")
    
    # Find crossover points
    print("\n3. Key Observations:")
    for n in df_mean['n'].unique():
        n_data = df_mean[df_mean['n'] == n]
        fastest = n_data.loc[n_data['time'].idxmin(), 'variant']
        if n in [32, 256, 1024, 4096]:  # Sample points for analysis
            print(f"   n={n}: Fastest variant is {fastest}")

def plot_results(csv_file):
    """Plot the results from the timing CSV file with enhanced analysis."""
    # Read the CSV file
    df = pd.read_csv(csv_file)
    
    # Calculate mean time and standard deviation for each n and variant
    df_mean = df.groupby(['n', 'variant'])['time'].agg(['mean', 'std']).reset_index()
    
    # Create the main performance plot
    plt.figure(figsize=(15, 10))
    
    # Create two subplots
    plt.subplot(2, 1, 1)
    
    # Plot with error bars
    for variant in df_mean['variant'].unique():
        data = df_mean[df_mean['variant'] == variant]
        plt.errorbar(data['n'], data['mean'], yerr=data['std'],
                    marker='o', label=variant, capsize=5)
    
    plt.xscale('log', base=2)
    plt.yscale('log')
    plt.xlabel('Number of Nodes (n)')
    plt.ylabel('Execution Time (seconds)')
    plt.title('Performance Comparison of Kruskal MST Variants')
    plt.legend()
    plt.grid(True)
    
    # Create relative performance plot
    plt.subplot(2, 1, 2)
    
    # Calculate relative performance (normalized to PC+Array)
    baseline = df_mean[df_mean['variant'] == 'PC+Array']['mean'].values
    for variant in df_mean['variant'].unique():
        if variant != 'PC+Array':
            data = df_mean[df_mean['variant'] == variant]
            relative_time = data['mean'] / baseline
            plt.plot(data['n'], relative_time, marker='o', label=f'{variant} vs PC+Array')
    
    plt.xscale('log', base=2)
    plt.xlabel('Number of Nodes (n)')
    plt.ylabel('Relative Time (compared to PC+Array)')
    plt.title('Relative Performance vs PC+Array')
    plt.legend()
    plt.grid(True)
    
    plt.tight_layout()
    plt.savefig('results/performance_analysis.png')
    plt.close()
    
    # Analyze results
    analyze_results(df)

def main():
    # Create results directory if it doesn't exist
    Path('results').mkdir(exist_ok=True)
    
    # Plot and analyze results
    plot_results('results/timing.csv')
    print("\nAnalysis plots saved as results/performance_analysis.png")

if __name__ == "__main__":
    main() 