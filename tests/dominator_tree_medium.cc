#include <string>
#include <iostream>
#include "gtest/gtest.h"
#include "graph.cc"

TEST(JayhawkTests, DominatorTreeMedium) {
  // Example from http://pages.cs.wisc.edu/~fischer/cs701.f08/lectures/Lecture19.4up.pdf
  Graph<std::string> cfg;
  cfg.add_node("Start");
  cfg.add_node("A");
  cfg.add_node("B");
  cfg.add_node("C");
  cfg.add_node("D");
  cfg.add_node("E");
  cfg.add_node("F");
  cfg.add_node("End");

  // Add edges
  cfg.add_edge("Start", "A");
  cfg.add_edge("A", "B");
  cfg.add_edge("A", "C");
  cfg.add_edge("B", "D");
  cfg.add_edge("C", "D");
  cfg.add_edge("D", "E");
  cfg.add_edge("E", "F");
  cfg.add_edge("F", "E");
  cfg.add_edge("F", "End");

  Graph<std::string> dominator_tree = cfg.copy_and_clear();

  // Add edges
  dominator_tree.add_edge("Start", "A");
  dominator_tree.add_edge("A", "B");
  dominator_tree.add_edge("A", "C");
  dominator_tree.add_edge("A", "D");
  dominator_tree.add_edge("D", "E");
  dominator_tree.add_edge("E", "F");
  dominator_tree.add_edge("F", "End");


  std::cout << "Original CFG \n" << cfg << "\n";
  std::cout << "Dominator Tree \n" << cfg.dominator_tree("Start") << "\n";
  ASSERT_EQ(cfg.dominator_tree("Start") == dominator_tree, true);
}
