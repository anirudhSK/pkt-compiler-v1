#include <iostream>
#include "gtest/gtest.h"
#include "graph.cc"

TEST(JayhawkTests, DominanceFrontier) {
  // Example from Fig. 19.4 b of Appel's book
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

  Graph<int> dominator_tree = cfg.copy_and_clear();

  // Add edges
  dominator_tree.add_edge(1, 2);
  dominator_tree.add_edge(2, 3);
  dominator_tree.add_edge(2, 4);
  dominator_tree.add_edge(3, 5);
  dominator_tree.add_edge(3, 6);
  dominator_tree.add_edge(3, 7);


  std::cout << "Original CFG \n" << cfg << "\n";
  std::cout << "Dominator Tree \n" << cfg.dominator_tree(1) << "\n";
  std::cout << "Dominance frontier\n";
  cfg.print_dominators(cfg.dominance_frontier(1));
}
