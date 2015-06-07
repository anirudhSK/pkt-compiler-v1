#include <iostream>
#include "dominator_utility.h"

template <class NodeType>
DominatorUtility<NodeType>::DominatorUtility(const Graph<NodeType> & t_graph,
                                             const NodeType & t_start_node)
    : graph_(t_graph),
      start_node_(t_start_node),
      dominators_(construct_dominators(graph_, start_node_)),
      dominator_tree_(construct_dom_tree(graph_, start_node_, dominators_)),
      dominance_frontier_(construct_dom_frontiers(graph_, dominator_tree_, dominators_)) {}

template <class NodeType>
auto DominatorUtility<NodeType>::construct_dominators(const Graph<NodeType> & t_graph,
                                                      const NodeType & t_start_node) {
  NodeSetMap dominators;
  dominators[t_start_node] = {t_start_node};
  for (const auto & node : (t_graph.node_set() - std::set<NodeType>{t_start_node})) {
    dominators[node] = t_graph.node_set();
  }

  NodeSetMap prev_dominators;
  while (dominators != prev_dominators) {
    // Save old dominators
    prev_dominators = dominators;

    // Run dataflow equations
    for (const auto & node : (t_graph.node_set() - std::set<NodeType>{t_start_node})) {
      std::set<NodeType> pred_intersection = t_graph.node_set();
      for (const auto & pred : t_graph.pred_map().at(node)) {
        pred_intersection = pred_intersection * dominators.at(pred);
      }
      dominators.at(node) = std::set<NodeType>{node} + pred_intersection;
    }
  }
  return dominators;
}

template <class NodeType>
auto DominatorUtility<NodeType>::construct_dom_tree(const Graph<NodeType> & t_graph,
                                                    const NodeType & t_start_node,
                                                    const NodeSetMap & t_dominators) {
  // Initialize dominator_tree_ based on graph
  auto dominator_tree = t_graph.copy_and_clear();

  // Connect idom(n) to n
  for (const auto & node : t_graph.node_set() - std::set<NodeType>{t_start_node}) {
    dominator_tree.add_edge(get_idom(node, t_dominators), node);
  }

  return dominator_tree;
}

template <class NodeType>
auto DominatorUtility<NodeType>::construct_dom_frontiers(const Graph<NodeType> & t_graph,
                                                         const Graph<NodeType> & t_dom_tree,
                                                         const NodeSetMap & t_dominators) {
  NodeSetMap dominance_frontier; 
  for (const auto & node : t_graph.node_set()) {
    dominance_frontier[node] = dom_frontier_helper(node, t_graph, t_dom_tree, t_dominators);
  }
  return dominance_frontier;
}

template <class NodeType>
typename DominatorUtility<NodeType>::NodeSet DominatorUtility<NodeType>::dom_frontier_helper(const NodeType & node,
                                                                                             const Graph<NodeType> & t_graph,
                                                                                             const Graph<NodeType> & t_dom_tree,
                                                                                             const NodeSetMap & t_dominators) {
  NodeSet S;
  for (const auto & y : t_graph.succ_map().at(node)) {
    assert(t_dom_tree.pred_map().at(y).size() == 1);
    if (t_dom_tree.pred_map().at(y).front() != node) {
      S = S + std::set<NodeType>{y};
    }
  }

  for (const auto & child : t_dom_tree.succ_map().at(node)) {
    const auto frontier_child = dom_frontier_helper(child, t_graph, t_dom_tree, t_dominators);
    for (const auto & w : frontier_child) {
      // if: w's set of dominators does not contain node
      // if: w is node
      if ((t_dominators.at(w).find(node) == t_dominators.at(w).end()) or
          (node == w)) {
        S = S + std::set<NodeType>{w};
      }
    }
  }
  // If we get here, the results were not memoized to begin with
  return S;
}

template <class NodeType>
void DominatorUtility<NodeType>::print_dominators() const {
  for (const auto & node : dominators_) {
    std::cout << node.first << " dominated by ";
    for (const auto & dom_node : node.second) {
      std::cout << dom_node << " ";
    }
    std::cout << "\n";
  }
}

template <class NodeType>
auto DominatorUtility<NodeType>::get_idom(const NodeType & node, const NodeSetMap & dominators) {
  // Naive implementation of Page 380 of Appel's book

  std::vector<NodeType> idoms;
  // 2. (from page 380) idom must dominate node
  for (const auto & idom_candidate : dominators.at(node)) {
    if (idom_candidate == node) {
      // 1. (from page 380) idom can't be node itself
      continue;
    } else {
      // 3. idom should not dominate some other dominator of node
      bool dominates_other = false;
      for (const auto & other_doms : dominators.at(node) - std::set<NodeType>{idom_candidate, node}) {
        if (dominators.at(other_doms).find(idom_candidate) != dominators.at(other_doms).end()) {
          dominates_other = true;
          break;
        }
      }
      if (dominates_other == false) idoms.emplace_back(idom_candidate);
    }
  }
  // There has to be exactly one idom (Page 380 of Appel's book)
  assert(idoms.size() == 1);
  return idoms.front();
}
