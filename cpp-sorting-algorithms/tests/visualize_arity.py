try:
    import pandas as pd
    import matplotlib.pyplot as plt
    import numpy as np
    import os
except ImportError as e:
    print(f"Error importing required libraries: {e}")
    print("Please install the required packages with: pip install pandas matplotlib numpy")
    exit(1)

# Check if results file exists
if not os.path.exists('data/optimal_arity_results.csv'):
    print("No arity experiment results found. Run the optimal_arity_experiment first.")
    exit(1)

# Create results directory if it doesn't exist
os.makedirs('results', exist_ok=True)

# Load the data
try:
    df = pd.read_csv('data/optimal_arity_results.csv')
    print("Available columns in the CSV:", df.columns.tolist())
    
    # Display first few rows to see data format
    print("\nFirst few rows of data:")
    print(df.head())
    
    # Convert Time column to numeric if it's not already
    try:
        df['Time'] = pd.to_numeric(df['Time'], errors='coerce')
        print("\nConverted Time column to numeric. Any non-numeric values were set to NaN.")
        # Drop rows with NaN times
        df = df.dropna(subset=['Time'])
        print(f"Shape after dropping NaN values: {df.shape}")
    except Exception as e:
        print(f"Error converting Time column: {e}")
    
except Exception as e:
    print(f"Error reading results file: {e}")
    exit(1)

# Check for empty dataframe
if df.empty:
    print("No valid data found in the CSV file after processing. Check the file format.")
    exit(1)

# Get unique combinations of algorithm, data type, size, and memory
experiments = df[['Algorithm', 'DataType', 'Size', 'MemorySize']].drop_duplicates()

# Filter out the auto-tuned versions for these plots
df_standard = df[~df['Algorithm'].str.contains('-Auto', na=False)]

# Create various plots
def create_arity_performance_plot():
    plt.figure(figsize=(12, 8))
    
    # Plot individual points instead of using groupby
    for algorithm in df_standard['Algorithm'].unique():
        algorithm_data = df_standard[df_standard['Algorithm'] == algorithm]
        
        # Sort by arity for line plot
        algorithm_data = algorithm_data.sort_values('Arity')
        
        plt.plot(algorithm_data['Arity'], algorithm_data['Time'], 
                 marker='o', label=algorithm)
    
    plt.title('Algorithm Performance by Arity')
    plt.xlabel('Arity')
    plt.ylabel('Execution Time (seconds)')
    plt.legend()
    plt.grid(True)
    plt.savefig('results/arity_performance.png')
    plt.close()
    print("Created arity performance plot")

# Create a plot for disk operations vs arity
def create_disk_operations_plot():
    plt.figure(figsize=(12, 8))
    
    # Plot individual points for each algorithm
    for algorithm in df_standard['Algorithm'].unique():
        algo_data = df_standard[df_standard['Algorithm'] == algorithm]
        algo_data = algo_data.sort_values('Arity')
        
        plt.plot(algo_data['Arity'], algo_data['DiskReads'], 
                 marker='o', linestyle='-', label=f'{algorithm} Reads')
        plt.plot(algo_data['Arity'], algo_data['DiskWrites'], 
                 marker='s', linestyle='--', label=f'{algorithm} Writes')
    
    plt.title('Disk Operations by Arity')
    plt.xlabel('Arity')
    plt.ylabel('Number of Operations')
    plt.legend()
    plt.grid(True)
    plt.savefig('results/disk_operations.png')
    plt.close()
    print("Created disk operations plot")

# Create comparison between optimal and actual best arity
def create_optimal_comparison_plot():
    # For each algorithm, compare the theoretical optimal arity with the actual best performing arity
    results = []
    
    for algorithm in df_standard['Algorithm'].unique():
        algo_data = df_standard[df_standard['Algorithm'] == algorithm]
        
        # Get the theoretical optimal arity (should be the same for all rows with the same memory size)
        for mem_size in algo_data['MemorySize'].unique():
            mem_data = algo_data[algo_data['MemorySize'] == mem_size]
            
            if mem_data.empty:
                continue
                
            # Theoretical optimal arity
            optimal_arity = mem_data['OptimalArity'].iloc[0]
            
            # Actual best arity (with minimum execution time)
            if mem_data['Time'].isna().all():
                continue  # Skip if all times are NaN
                
            best_idx = mem_data['Time'].idxmin()
            best_arity = mem_data.loc[best_idx, 'Arity']
            best_time = mem_data.loc[best_idx, 'Time']
            
            # Performance at optimal arity
            optimal_rows = mem_data[mem_data['Arity'] == optimal_arity]
            if not optimal_rows.empty and not optimal_rows['Time'].isna().all():
                optimal_time = optimal_rows['Time'].iloc[0]
                
                results.append({
                    'Algorithm': algorithm,
                    'MemorySize': mem_size,
                    'OptimalArity': optimal_arity,
                    'BestArity': best_arity,
                    'OptimalTime': optimal_time,
                    'BestTime': best_time
                })
    
    if results:
        # Convert to DataFrame for easier plotting
        results_df = pd.DataFrame(results)
        
        # Create bar chart comparing optimal vs best
        x = np.arange(len(results_df))
        width = 0.35
        
        fig, ax = plt.subplots(figsize=(14, 8))
        rects1 = ax.bar(x - width/2, results_df['OptimalTime'], width, label='Theoretical Optimal')
        rects2 = ax.bar(x + width/2, results_df['BestTime'], width, label='Actual Best')
        
        # Add labels and title
        ax.set_xlabel('Algorithm & Memory Size')
        ax.set_ylabel('Execution Time (seconds)')
        ax.set_title('Theoretical Optimal vs Actual Best Arity Performance')
        ax.set_xticks(x)
        ax.set_xticklabels([f"{row['Algorithm']}\nM={row['MemorySize']}" for _, row in results_df.iterrows()])
        ax.legend()
        
        # Add text annotations
        for i, rect in enumerate(rects1):
            height = rect.get_height()
            if height > 0:  # Only annotate if there's a visible bar
                ax.annotate(f"a={results_df.iloc[i]['OptimalArity']}", 
                          (rect.get_x() + rect.get_width()/2, height),
                          ha='center', va='bottom', rotation=90)
            
        for i, rect in enumerate(rects2):
            height = rect.get_height()
            if height > 0:  # Only annotate if there's a visible bar
                ax.annotate(f"a={results_df.iloc[i]['BestArity']}", 
                          (rect.get_x() + rect.get_width()/2, height),
                          ha='center', va='bottom', rotation=90)
        
        plt.tight_layout()
        plt.savefig('results/optimal_vs_best.png')
        plt.close()
        print("Created optimal vs best comparison plot")
    else:
        print("No valid data for optimal vs best comparison plot")

try:
    # Call the functions to create plots
    create_arity_performance_plot()
    create_disk_operations_plot()
    create_optimal_comparison_plot()
    print("Visualization complete. Results saved in the 'results' directory.")
except Exception as e:
    print(f"Error creating visualizations: {e}")
    import traceback
    traceback.print_exc()