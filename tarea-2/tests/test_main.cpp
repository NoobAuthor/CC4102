// test_main.cpp
// Archivo principal para pruebas del proyecto
#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>

#include "../src/kruskal.h"
#include "../src/union_find.h"

// Helper function to check if two doubles are approximately equal
bool approx_equal(double a, double b, double epsilon = 1e-10) {
  return std::abs(a - b) < epsilon;
}

void test_union_find_basic() {
  std::cout << "\n[TEST] Testing UnionFind basic functionality...\n";

  // Test with path compression
  UnionFind uf_pc(5, true);
  // Test without path compression
  UnionFind uf_no_pc(5, false);

  // Test both variants
  for (auto* uf : {&uf_pc, &uf_no_pc}) {
    std::string variant = (uf == &uf_pc) ? "with PC" : "without PC";
    std::cout << "Testing " << variant << ":\n";

    // Test initial state
    for (int i = 0; i < 5; ++i) {
      assert(uf->find(i) == i);
    }

    // Test unions and connectivity
    uf->unite(0, 1);
    assert(uf->connected(0, 1));

    uf->unite(2, 3);
    assert(uf->connected(2, 3));
    assert(!uf->connected(1, 2));

    uf->unite(1, 2);
    assert(uf->connected(0, 2));
    assert(uf->connected(1, 3));

    uf->unite(4, 0);
    for (int i = 0; i < 5; ++i) {
      for (int j = 0; j < 5; ++j) {
        assert(uf->connected(i, j));
      }
    }
  }
  std::cout << "Basic UnionFind tests passed!\n";
}

void test_kruskal_small_graph() {
  std::cout << "\n[TEST] Testing Kruskal's algorithm on small graph...\n";

  // Create a simple graph where we know the MST
  std::vector<Edge> edges = {
      Edge(0, 1, 1.0),  // Include this
      Edge(1, 2, 2.0),  // Include this
      Edge(2, 0, 3.0),  // Don't include this
      Edge(2, 3, 1.5),  // Include this
      Edge(3, 0, 4.0)   // Don't include this
  };

  const int n = 4;
  const double expected_weight = 4.5;  // 1.0 + 2.0 + 1.5

  // Test all variants
  std::vector<std::pair<bool, Kruskal::EdgeSelection>> variants = {
      {true, Kruskal::EdgeSelection::SORTED_ARRAY},
      {true, Kruskal::EdgeSelection::HEAP},
      {false, Kruskal::EdgeSelection::SORTED_ARRAY},
      {false, Kruskal::EdgeSelection::HEAP}};

  for (const auto& [use_pc, edge_sel] : variants) {
    std::string variant =
        std::string(use_pc ? "PC+" : "NoPC+") +
        (edge_sel == Kruskal::EdgeSelection::SORTED_ARRAY ? "Array" : "Heap");
    std::cout << "Testing " << variant << ":\n";

    Kruskal kruskal(n, edges, use_pc, edge_sel);
    auto [weight, mst] = kruskal.run();

    // Check total weight
    assert(approx_equal(weight, expected_weight));

    // Check number of edges in MST
    assert(mst.size() == n - 1);

    // Check connectivity
    UnionFind uf(n, true);
    for (const auto& edge : mst) {
      uf.unite(edge.u, edge.v);
    }
    for (int i = 0; i < n; ++i) {
      for (int j = 0; j < n; ++j) {
        assert(uf.connected(i, j));
      }
    }
  }
  std::cout << "Small graph Kruskal tests passed!\n";
}

void test_kruskal_special_cases() {
  std::cout << "\n[TEST] Testing Kruskal's algorithm special cases...\n";

  // Test empty graph
  {
    std::vector<Edge> edges;
    Kruskal kruskal(0, edges, true, Kruskal::EdgeSelection::SORTED_ARRAY);
    auto [weight, mst] = kruskal.run();
    assert(weight == 0.0);
    assert(mst.empty());
  }

  // Test single node
  {
    std::vector<Edge> edges;
    Kruskal kruskal(1, edges, true, Kruskal::EdgeSelection::SORTED_ARRAY);
    auto [weight, mst] = kruskal.run();
    assert(weight == 0.0);
    assert(mst.empty());
  }

  // Test disconnected graph
  {
    std::vector<Edge> edges = {Edge(0, 1, 1.0), Edge(2, 3, 1.0)};
    Kruskal kruskal(4, edges, true, Kruskal::EdgeSelection::SORTED_ARRAY);
    auto [weight, mst] = kruskal.run();
    assert(mst.size() == 2);  // Should include both edges
    assert(approx_equal(weight, 2.0));
  }

  std::cout << "Special cases tests passed!\n";
}

int main() {
  try {
    test_union_find_basic();
    test_kruskal_small_graph();
    test_kruskal_special_cases();

    std::cout << "\n[SUCCESS] All tests passed successfully!\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "\n[FAILURE] Test failed with error: " << e.what() << "\n";
    return 1;
  }
}