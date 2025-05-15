#pragma once
#include <queue>
#include <vector>

#include "union_find.h"

// Estructura para representar una arista
struct Edge {
  int u, v;
  double weight;

  // Constructor
  Edge(int u = 0, int v = 0, double weight = 0.0)
      : u(u), v(v), weight(weight) {}

  // Operador de comparación para la cola de prioridad
  bool operator>(const Edge& other) const { return weight > other.weight; }
};

class Kruskal {
 public:
  enum class EdgeSelection { SORTED_ARRAY, HEAP };

  /**
   * Constructor
   * @param n Número de nodos
   * @param edges Lista de aristas
   * @param usePathCompression Si se debe usar compresión de caminos en
   * Union-Find
   * @param edgeSelection Método de selección de aristas (array ordenado o heap)
   */
  Kruskal(int n, const std::vector<Edge>& edges, bool usePathCompression,
          EdgeSelection edgeSelection);

  /**
   * Ejecuta el algoritmo de Kruskal y retorna el MST
   * @return Par con el peso total y las aristas del MST
   */
  std::pair<double, std::vector<Edge>> run();

 private:
  int n;                        // Número de nodos
  std::vector<Edge> edges;      // Lista de aristas
  bool usePathCompression;      // Flag para Union-Find
  EdgeSelection edgeSelection;  // Método de selección de aristas

  // Métodos auxiliares para cada variante
  std::pair<double, std::vector<Edge>> runWithSortedArray();
  std::pair<double, std::vector<Edge>> runWithHeap();
};