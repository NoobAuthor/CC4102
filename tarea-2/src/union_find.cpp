// union_find.cpp
// Implementación de la estructura Union-Find (Disjoint Set)
#include "union_find.h"

/**
 * Constructor: inicializa n conjuntos disjuntos.
 */
UnionFind::UnionFind(int n, bool usePathCompression)
    : parent(n), rank(n, 0), usePathCompression(usePathCompression) {
  for (int i = 0; i < n; ++i) {
    parent[i] = i;
  }
}

/**
 * Encuentra el representante del conjunto que contiene a x.
 * Implementa dos variantes: con y sin compresión de caminos.
 */
int UnionFind::find(int x) {
  if (usePathCompression) {
    // Versión con compresión de caminos
    if (parent[x] != x) {
      parent[x] = find(parent[x]);
    }
    return parent[x];
  } else {
    // Versión sin compresión de caminos
    while (x != parent[x]) {
      x = parent[x];
    }
    return x;
  }
}

/**
 * Une los conjuntos que contienen a x e y (por rango).
 */
void UnionFind::unite(int x, int y) {
  int rootX = find(x);
  int rootY = find(y);
  if (rootX == rootY) return;
  // Union by rank
  if (rank[rootX] < rank[rootY]) {
    parent[rootX] = rootY;
  } else if (rank[rootX] > rank[rootY]) {
    parent[rootY] = rootX;
  } else {
    parent[rootY] = rootX;
    rank[rootX]++;
  }
}

/**
 * Verifica si x e y están en el mismo conjunto.
 */
bool UnionFind::connected(int x, int y) { return find(x) == find(y); }