#!/usr/bin/env python3
import numpy as np
import os
from pathlib import Path

def generate_points(n, seed=None):
    """Generate n random points in [0,1]²."""
    if seed is not None:
        np.random.seed(seed)
    return np.random.random((n, 2))

def compute_edges(points):
    """Compute all edges between points with squared Euclidean distance as weight."""
    n = len(points)
    edges = []
    for i in range(n):
        for j in range(i + 1, n):
            # Squared Euclidean distance
            weight = np.sum((points[i] - points[j]) ** 2)
            edges.append((i, j, weight))
    return edges

def save_graph(points, edges, filename):
    """Save the graph to a file."""
    n = len(points)
    m = len(edges)
    
    with open(filename, 'w') as f:
        # First line: number of vertices and edges
        f.write(f"{n} {m}\n")
        
        # Write points
        for x, y in points:
            f.write(f"{x} {y}\n")
        
        # Write edges
        for u, v, w in edges:
            f.write(f"{u} {v} {w}\n")

def main():
    # Create directories if they don't exist
    input_dir = Path("input")
    input_dir.mkdir(exist_ok=True)
    
    # Generate for each size
    sizes = [2**i for i in range(5, 13)]  # 2^5 to 2^12
    runs = 5
    
    for n in sizes:
        print(f"Generating for n={n}...")
        for run in range(runs):
            # Generate points with different seeds
            seed = n * 100 + run
            points = generate_points(n, seed)
            edges = compute_edges(points)
            
            # Save to file
            filename = input_dir / f"input_n{n}_run{run}.txt"
            save_graph(points, edges, filename)
            print(f"  Saved {filename}")

if __name__ == "__main__":
    main() 