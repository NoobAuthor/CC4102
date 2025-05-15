// test_main.cpp
// Archivo principal para pruebas del proyecto
#include <cassert>
#include <iostream>

#include "../src/union_find.h"

int main() {
  std::cout << "[TEST] Iniciando pruebas de UnionFind...\n";
  UnionFind uf(5);
  std::cout << "[TEST] Verificando conjuntos iniciales...\n";
  // Inicialmente, todos los elementos están en conjuntos separados
  for (int i = 0; i < 5; ++i) {
    assert(uf.find(i) == i);
  }
  std::cout << "[TEST] Uniendo 0 y 1...\n";
  uf.unite(0, 1);
  assert(uf.connected(0, 1));
  std::cout << "[TEST] Uniendo 1 y 2...\n";
  uf.unite(1, 2);
  assert(uf.connected(0, 2));
  assert(!uf.connected(0, 3));
  std::cout << "[TEST] Uniendo 3 y 4...\n";
  uf.unite(3, 4);
  assert(uf.connected(3, 4));
  std::cout << "[TEST] Uniendo 2 y 4...\n";
  uf.unite(2, 4);
  std::cout << "[TEST] Verificando que todos estén conectados...\n";
  // Ahora todos deben estar conectados
  for (int i = 0; i < 5; ++i) {
    for (int j = 0; j < 5; ++j) {
      assert(uf.connected(i, j));
    }
  }
  std::cout << "[TEST] Todas las pruebas pasaron correctamente.\n";
  return 0;
}