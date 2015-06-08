#include <map>
#include <set>
#include <iostream>
#include "gtest/gtest.h"
#include "graph.cc"
#include "dominator_utility.cc"

TEST(JayhawkTests, ControlDependenceGraph) {
  // Example from Fig. 19.5 of Appel's book
  Graph<int> cfg;
  for (int i = 1; i <= 7; i++) {
    cfg.add_node(i);
  }

  // Add edges
  cfg.add_edge(1, 2);
  cfg.add_edge(2, 3);
  cfg.add_edge(2, 4);
  cfg.add_edge(3, 5);
  cfg.add_edge(3, 6);
  cfg.add_edge(5, 7);
  cfg.add_edge(6, 7);
  cfg.add_edge(7, 2);

  // Add an entry (-1) and exit node (100)
  cfg.add_node(-1);
  cfg.add_node(100);

  // Add edges to these nodes
  cfg.add_edge(-1, 1);
  cfg.add_edge(4, 100);
  cfg.add_edge(-1, 100);
  
  // Compute control dependence graph
  Graph<int> cdg;
  for (int i = 1; i <= 7; i++) {
    cdg.add_node(i);
  }
  cdg.add_node(-1);

  for (const auto & y : DominatorUtility<int>(cfg.transpose(), 100).dominance_frontier()) {
    for (const auto & x : y.second) {
      // Item # 5 on page 426
      cdg.add_edge(x, y.first);
    }
  }

  // Expected cdg
  Graph<int> expected_cdg;
  for (int i = 1; i <= 7; i++) {
    expected_cdg.add_node(i);
  }
  expected_cdg.add_node(-1);
  expected_cdg.add_edge(-1, 2);
  expected_cdg.add_edge(-1, 1);
  expected_cdg.add_edge(-1, 4);
  expected_cdg.add_edge(2, 2);
  expected_cdg.add_edge(2, 3);
  expected_cdg.add_edge(2, 7);
  expected_cdg.add_edge(3, 5);
  expected_cdg.add_edge(3, 6);

  std::cout << "Original CFG \n" << cfg << "\n";
  std::cout << "CDG \n" << cdg << "\n";
  std::cout << "expected_cdg \n" << expected_cdg << "\n";
  ASSERT_EQ(cdg == expected_cdg, true);
}
