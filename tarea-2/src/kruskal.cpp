#include "kruskal.h"

#include <algorithm>

Kruskal::Kruskal(int n, const std::vector<Edge>& edges, bool usePathCompression,
                 EdgeSelection edgeSelection)
    : n(n),
      edges(edges),
      usePathCompression(usePathCompression),
      edgeSelection(edgeSelection) {}

std::pair<double, std::vector<Edge>> Kruskal::run() {
  if (edgeSelection == EdgeSelection::SORTED_ARRAY) {
    return runWithSortedArray();
  } else {
    return runWithHeap();
  }
}

std::pair<double, std::vector<Edge>> Kruskal::runWithSortedArray() {
  // Ordenar aristas por peso
  std::vector<Edge> sortedEdges = edges;
  std::sort(sortedEdges.begin(), sortedEdges.end(),
            [](const Edge& a, const Edge& b) { return a.weight < b.weight; });

  UnionFind uf(n, usePathCompression);
  std::vector<Edge> mst;
  double totalWeight = 0.0;

  for (const auto& edge : sortedEdges) {
    if (!uf.connected(edge.u, edge.v)) {
      uf.unite(edge.u, edge.v);
      mst.push_back(edge);
      totalWeight += edge.weight;
    }
  }

  return {totalWeight, mst};
}

std::pair<double, std::vector<Edge>> Kruskal::runWithHeap() {
  // Crear min-heap de aristas
  std::priority_queue<Edge, std::vector<Edge>, std::greater<Edge>> pq;
  for (const auto& edge : edges) {
    pq.push(edge);
  }

  UnionFind uf(n, usePathCompression);
  std::vector<Edge> mst;
  double totalWeight = 0.0;
  const size_t target_size = static_cast<size_t>(n - 1);

  while (!pq.empty() && mst.size() < target_size) {
    Edge edge = pq.top();
    pq.pop();

    if (!uf.connected(edge.u, edge.v)) {
      uf.unite(edge.u, edge.v);
      mst.push_back(edge);
      totalWeight += edge.weight;
    }
  }

  return {totalWeight, mst};
}