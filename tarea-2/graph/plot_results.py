#!/usr/bin/env python3
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from pathlib import Path

def plot_results(csv_file):
    """Plot the results from the timing CSV file."""
    # Read the CSV file
    df = pd.read_csv(csv_file)
    
    # Calculate mean time for each n and variant
    df_mean = df.groupby(['n', 'variant'])['time'].mean().reset_index()
    
    # Create the plot
    plt.figure(figsize=(12, 8))
    sns.set_style("whitegrid")
    
    # Plot one line per variant
    for variant in df_mean['variant'].unique():
        data = df_mean[df_mean['variant'] == variant]
        plt.plot(data['n'], data['time'], marker='o', label=variant)
    
    plt.xscale('log', base=2)  # Use log scale for x-axis
    plt.yscale('log')  # Use log scale for y-axis
    
    plt.xlabel('Number of Nodes (n)')
    plt.ylabel('Execution Time (seconds)')
    plt.title('Kruskal MST Algorithm Variants Performance')
    plt.legend()
    plt.grid(True)
    
    # Save the plot
    plt.savefig('results/performance_plot.png')
    plt.close()

def main():
    # Create results directory if it doesn't exist
    Path('results').mkdir(exist_ok=True)
    
    # Plot results
    plot_results('results/timing.csv')
    print("Plot saved as results/performance_plot.png")

if __name__ == "__main__":
    main() 