// main.cpp
// Entry point for the project
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "kruskal.h"
#include "union_find.h"

// Función para leer el grafo desde un archivo
std::pair<int, std::vector<Edge>> read_graph(const std::string& filename) {
  std::ifstream fin(filename);
  int n, m;
  fin >> n >> m;

  // Leer puntos (no los usamos directamente, pero están en el archivo)
  double x, y;
  for (int i = 0; i < n; i++) {
    fin >> x >> y;
  }

  // Leer aristas
  std::vector<Edge> edges;
  int u, v;
  double w;
  for (int i = 0; i < m; i++) {
    fin >> u >> v >> w;
    edges.emplace_back(u, v, w);
  }

  return {n, edges};
}

// Función para ejecutar una variante de Kruskal y medir el tiempo
double run_variant(int n, const std::vector<Edge>& edges,
                   bool usePathCompression,
                   Kruskal::EdgeSelection edgeSelection) {
  auto start = std::chrono::high_resolution_clock::now();

  Kruskal kruskal(n, edges, usePathCompression, edgeSelection);
  auto [weight, mst] = kruskal.run();

  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start);

  return duration.count() / 1000000.0;  // Convertir a segundos
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Uso: " << argv[0] << " <archivo_entrada>\n";
    return 1;
  }

  // Leer el grafo
  auto [n, edges] = read_graph(argv[1]);

  // Ejecutar las 4 variantes y medir tiempo
  std::vector<std::pair<std::string, double>> results;

  // 1. Path compression + sorted array
  results.emplace_back(
      "PC+Array",
      run_variant(n, edges, true, Kruskal::EdgeSelection::SORTED_ARRAY));

  // 2. Path compression + heap
  results.emplace_back(
      "PC+Heap", run_variant(n, edges, true, Kruskal::EdgeSelection::HEAP));

  // 3. No path compression + sorted array
  results.emplace_back(
      "NoPC+Array",
      run_variant(n, edges, false, Kruskal::EdgeSelection::SORTED_ARRAY));

  // 4. No path compression + heap
  results.emplace_back(
      "NoPC+Heap", run_variant(n, edges, false, Kruskal::EdgeSelection::HEAP));

  // Imprimir resultados
  std::cout << "Resultados para n = " << n << ":\n";
  for (const auto& [variant, time] : results) {
    std::cout << variant << ": " << time << " segundos\n";
  }

  // Guardar resultados en CSV
  std::string results_file = "results/timing.csv";
  bool file_exists = std::ifstream(results_file).good();

  std::ofstream fout(results_file, std::ios::app);
  if (!file_exists) {
    fout << "n,variant,time\n";
  }

  for (const auto& [variant, time] : results) {
    fout << n << "," << variant << "," << time << "\n";
  }

  return 0;
}