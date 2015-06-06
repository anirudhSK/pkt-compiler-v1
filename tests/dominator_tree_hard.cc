#include <iostream>
#include "gtest/gtest.h"
#include "graph.cc"

TEST(JayhawkTests, DominatorTreeHard) {
  // Harder example from Fig. 18.3 of Appel's book
  Graph<int> cfg;
  for (int i = 1; i <= 12; i++) {
    cfg.add_node(i);
  }

  // Add edges
  cfg.add_edge(1, 2);
  cfg.add_edge(2, 3);
  cfg.add_edge(3, 2);
  cfg.add_edge(2, 4);
  cfg.add_edge(4, 2);
  cfg.add_edge(4, 5);
  cfg.add_edge(4, 6);
  cfg.add_edge(5, 8);
  cfg.add_edge(5, 7);
  cfg.add_edge(6, 7);
  cfg.add_edge(8, 9);
  cfg.add_edge(9, 8);
  cfg.add_edge(7, 11);
  cfg.add_edge(11, 12);
  cfg.add_edge(9, 10);
  cfg.add_edge(10, 12);
  cfg.add_edge(10, 5);

  Graph<int> dominator_tree = cfg.copy_and_clear();

  // Add edges
  dominator_tree.add_edge(1, 2);
  dominator_tree.add_edge(2, 3);
  dominator_tree.add_edge(2, 4);
  dominator_tree.add_edge(4, 5);
  dominator_tree.add_edge(4, 6);
  dominator_tree.add_edge(4, 7);
  dominator_tree.add_edge(4, 12);
  dominator_tree.add_edge(5, 8);
  dominator_tree.add_edge(8, 9);
  dominator_tree.add_edge(9, 10);
  dominator_tree.add_edge(7, 11);

  std::cout << "Original CFG \n" << cfg << "\n";
  std::cout << "Dominator Tree \n" << cfg.dominator_tree(1) << "\n";
  ASSERT_EQ(cfg.dominator_tree(1) == dominator_tree, true);
}
