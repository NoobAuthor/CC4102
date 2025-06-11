// union_find.h
// Declaraciones para la estructura Union-Find (Disjoint Set)
#pragma once
#include <vector>

/**
 * Clase para la estructura de conjuntos disjuntos (Union-Find).
 */
class UnionFind {
 public:
  /**
   * Constructor: inicializa n conjuntos disjuntos.
   * @param n Número de elementos.
   * @param usePathCompression Si se debe usar compresión de caminos.
   */
  UnionFind(int n, bool usePathCompression = true);

  /**
   * Encuentra el representante del conjunto que contiene a x.
   * @param x Elemento a buscar.
   * @return Representante del conjunto.
   */
  int find(int x);

  /**
   * Une los conjuntos que contienen a x e y.
   * @param x Primer elemento.
   * @param y Segundo elemento.
   */
  void unite(int x, int y);

  /**
   * Verifica si x e y están en el mismo conjunto.
   * @param x Primer elemento.
   * @param y Segundo elemento.
   * @return true si están en el mismo conjunto, false en caso contrario.
   */
  bool connected(int x, int y);

 private:
  std::vector<int> parent;  // Vector de padres
  std::vector<int> rank;    // Vector de rangos para optimización
  bool usePathCompression;  // Flag para usar o no compresión de caminos
};