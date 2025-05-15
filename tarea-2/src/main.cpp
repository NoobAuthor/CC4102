// main.cpp
// Entry point for the project
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "union_find.h"

// Estructura para representar una arista (temporal, asumiendo entrada desde
// Python o archivo)
struct Edge {
  int u, v, weight;
};

int main(int argc, char* argv[]) {
  // Determine output directory
  std::string out_dir = (argc > 1) ? argv[1] : "results";
  std::string out_file = out_dir + "/mst_output.txt";

  // 1. Leer el grafo (número de nodos y lista de aristas)
  int n, m;  // n = número de nodos, m = número de aristas
  std::cin >> n >> m;
  std::vector<Edge> edges(m);
  for (int i = 0; i < m; ++i) {
    std::cin >> edges[i].u >> edges[i].v >> edges[i].weight;
  }

  // 2. Ordenar las aristas por peso
  std::sort(edges.begin(), edges.end(),
            [](const Edge& a, const Edge& b) { return a.weight < b.weight; });

  // 3. Inicializar Union-Find
  UnionFind uf(n);

  // 4. Ejecutar Kruskal para encontrar el MST
  std::vector<Edge> mst;
  int totalWeight = 0;
  for (const auto& edge : edges) {
    if (!uf.connected(edge.u, edge.v)) {
      uf.unite(edge.u, edge.v);
      mst.push_back(edge);
      totalWeight += edge.weight;
    }
  }

  // 5. Salida del MST a archivo en results/
  std::ofstream fout(out_file);
  fout << totalWeight << "\n";
  for (const auto& edge : mst) {
    fout << edge.u << " " << edge.v << " " << edge.weight << "\n";
  }
  fout.close();

  // También salida a stdout para referencia
  std::cout << "Peso total del MST: " << totalWeight << std::endl;
  std::cout << "Aristas del MST:" << std::endl;
  for (const auto& edge : mst) {
    std::cout << edge.u << " " << edge.v << " " << edge.weight << std::endl;
  }

  // Guardar resumen en results/summary.txt
  std::ofstream summary(out_dir + "/summary.txt");
  summary << "Nodos: " << n << "\n";
  summary << "Aristas: " << m << "\n";
  summary << "Peso total del MST: " << totalWeight << "\n";
  summary << "Aristas del MST:\n";
  for (const auto& edge : mst) {
    summary << edge.u << " " << edge.v << " " << edge.weight << "\n";
  }
  summary.close();

  return 0;
}