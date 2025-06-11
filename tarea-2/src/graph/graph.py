import networkx as nx
import matplotlib.pyplot as plt
import sys
import os
import csv


def read_mst(filename):
    """
    Reads the MST result from a file.
    :param filename: Path to the MST output file.
    :return: total_weight (int), list of edges [(u, v, weight)]
    """
    edges = []
    with open(filename, 'r') as f:
        lines = f.readlines()
        total_weight = int(lines[0].strip())
        for line in lines[1:]:
            u, v, w = map(int, line.strip().split())
            edges.append((u, v, w))
    return total_weight, edges


def read_input_graph(input_file):
    """
    Reads the original graph from the input file.
    :param input_file: Path to the input file.
    :return: n (int), m (int), list of edges [(u, v, weight)]
    """
    edges = []
    with open(input_file, 'r') as f:
        n, m = map(int, f.readline().strip().split())
        for line in f:
            u, v, w = map(int, line.strip().split())
            edges.append((u, v, w))
    return n, m, edges


def plot_graphs(original_edges, mst_edges, total_weight, results_dir):
    """
    Plots the original graph and the MST side by side.
    Saves both plots in the results directory.
    """
    # Build original graph
    G_orig = nx.Graph()
    for u, v, w in original_edges:
        G_orig.add_edge(u, v, weight=w)
    # Build MST graph
    G_mst = nx.Graph()
    for u, v, w in mst_edges:
        G_mst.add_edge(u, v, weight=w)

    pos = nx.shell_layout(G_orig)
    weights_orig = nx.get_edge_attributes(G_orig, 'weight')
    weights_mst = nx.get_edge_attributes(G_mst, 'weight')

    fig, axes = plt.subplots(1, 2, figsize=(18, 8))
    # Original graph
    plt.sca(axes[0])
    nx.draw_networkx_nodes(G_orig, pos, node_color='skyblue', node_size=900, edgecolors='black')
    nx.draw_networkx_edges(G_orig, pos, width=2, edge_color='gray')
    nx.draw_networkx_edge_labels(G_orig, pos, edge_labels=weights_orig, font_color='blue', font_size=12, bbox=dict(facecolor='white', edgecolor='none', boxstyle='round,pad=0.2'))
    nx.draw_networkx_labels(G_orig, pos, font_size=14, font_color='black', font_weight='bold')
    plt.title('Grafo Original', fontsize=16, fontweight='bold')
    plt.axis('off')
    # MST graph
    plt.sca(axes[1])
    nx.draw_networkx_nodes(G_mst, pos, node_color='skyblue', node_size=900, edgecolors='black')
    nx.draw_networkx_edges(G_mst, pos, width=4, edge_color='red')
    nx.draw_networkx_edge_labels(G_mst, pos, edge_labels=weights_mst, font_color='blue', font_size=12, bbox=dict(facecolor='white', edgecolor='none', boxstyle='round,pad=0.2'))
    nx.draw_networkx_labels(G_mst, pos, font_size=14, font_color='black', font_weight='bold')
    plt.title(f"MST (Peso total: {total_weight})", fontsize=16, fontweight='bold', color='darkred')
    plt.axis('off')
    plt.tight_layout()
    out_path = os.path.join(results_dir, 'comparison.png')
    plt.savefig(out_path, dpi=200)
    print(f"[INFO] Comparación guardada como '{out_path}'")
    plt.show()

    # Also save the MST alone as before
    plt.figure(figsize=(10, 8))
    nx.draw_networkx_nodes(G_mst, pos, node_color='skyblue', node_size=900, edgecolors='black')
    nx.draw_networkx_edges(G_mst, pos, width=4, edge_color='red')
    nx.draw_networkx_edge_labels(G_mst, pos, edge_labels=weights_mst, font_color='blue', font_size=12, bbox=dict(facecolor='white', edgecolor='none', boxstyle='round,pad=0.2'))
    nx.draw_networkx_labels(G_mst, pos, font_size=14, font_color='black', font_weight='bold')
    plt.title(f"Árbol de Expansión Mínima (Peso total: {total_weight})", fontsize=16, fontweight='bold', color='darkred', pad=20)
    plt.axis('off')
    plt.tight_layout()
    out_path2 = os.path.join(results_dir, 'mst_visualization.png')
    plt.savefig(out_path2, dpi=200)
    print(f"[INFO] Visualización MST guardada como '{out_path2}'")
    plt.close()


def save_mst_csv(mst_edges, total_weight, results_dir):
    csv_path = os.path.join(results_dir, 'mst_edges.csv')
    with open(csv_path, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerow(['u', 'v', 'weight'])
        for u, v, w in mst_edges:
            writer.writerow([u, v, w])
        writer.writerow([])
        writer.writerow(['Total Weight', total_weight])
    print(f"[INFO] MST guardado como CSV en '{csv_path}'")


def main():
    results_dir = sys.argv[1] if len(sys.argv) > 1 else 'results'
    os.makedirs(results_dir, exist_ok=True)
    input_file = 'input.txt'
    mst_file = os.path.join(results_dir, 'mst_output.txt')
    total_weight, mst_edges = read_mst(mst_file)
    n, m, original_edges = read_input_graph(input_file)
    plot_graphs(original_edges, mst_edges, total_weight, results_dir)
    save_mst_csv(mst_edges, total_weight, results_dir)


if __name__ == "__main__":
    main()
