#include <iostream>
#include "gtest/gtest.h"
#include "graph.cc"

TEST(JayhawkTests, FlippedCfg) {
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

  Graph<int> flipped_cfg = cfg.copy_and_clear();

  // Add edges
  flipped_cfg.add_edge(2, 1);
  flipped_cfg.add_edge(3, 2);
  flipped_cfg.add_edge(4, 2);
  flipped_cfg.add_edge(5, 3);
  flipped_cfg.add_edge(6, 3);
  flipped_cfg.add_edge(7, 5);
  flipped_cfg.add_edge(7, 6);
  flipped_cfg.add_edge(2, 7);

  std::cout << "Original CFG \n" << cfg << "\n";
  std::cout << "Flipped CFG \n" << cfg.transpose() << "\n";

  ASSERT_EQ(cfg.transpose() == flipped_cfg, true);
}
