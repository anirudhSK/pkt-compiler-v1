#include <map>
#include <set>
#include <iostream>
#include "gtest/gtest.h"
#include "graph.cc"
#include "dominator_utility.cc"

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

  // Add edges
  std::map<int, std::set<int>> expected_dom_frontier;
  expected_dom_frontier[1] = {};
  expected_dom_frontier[2] = {2};
  expected_dom_frontier[3] = {2};
  expected_dom_frontier[4] = {};
  expected_dom_frontier[5] = {7};
  expected_dom_frontier[6] = {7};
  expected_dom_frontier[7] = {2};


  std::cout << "Original CFG \n" << cfg << "\n";
  std::cout << "Dominator Tree \n" << DominatorUtility<int>(cfg, 1).dominator_tree() << "\n";
  ASSERT_EQ(DominatorUtility<int>(cfg, 1).dominance_frontier() == expected_dom_frontier, true);
}
