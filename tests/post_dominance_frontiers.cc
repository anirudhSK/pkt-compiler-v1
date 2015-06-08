#include <map>
#include <set>
#include <iostream>
#include "gtest/gtest.h"
#include "graph.cc"
#include "dominator_utility.cc"

TEST(JayhawkTests, PostDomFrontiers) {
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
  

  std::map<int, std::set<int>> expected_post_dom_frontier;
  expected_post_dom_frontier[-1] = {};
  expected_post_dom_frontier[1] = {-1};
  expected_post_dom_frontier[2] = {2, -1};
  expected_post_dom_frontier[3] = {2};
  expected_post_dom_frontier[4] = {-1};
  expected_post_dom_frontier[5] = {3};
  expected_post_dom_frontier[6] = {3};
  expected_post_dom_frontier[7] = {2};
  expected_post_dom_frontier[100] = {};

  std::cout << "Original CFG \n" << cfg << "\n";
  std::cout << "Post-dominator Tree \n" << DominatorUtility<int>(cfg.transpose(), 100).dominator_tree() << "\n";
  ASSERT_EQ(DominatorUtility<int>(cfg.transpose(), 100).dominance_frontier() == expected_post_dom_frontier, true);
}
