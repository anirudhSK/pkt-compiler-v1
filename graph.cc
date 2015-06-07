#include <iostream>
#include <cassert>
#include <stdexcept>
#include "graph.h"
#include "set_idioms.h"

template <class NodeType>
void Graph<NodeType>::add_node(const NodeType & node) {
  // Add node to node_set_
  if (node_set_.find(node) != node_set_.end()) {
    throw std::logic_error("Trying to insert node that already exists in node_set_\n");
  }
  node_set_.insert(node);
  succ_map_[node] = std::vector<NodeType>();
  pred_map_[node] = std::vector<NodeType>();
}

template <class NodeType>
void Graph<NodeType>::add_edge(const NodeType & from_node, const NodeType & to_node) {
  if (node_set_.find(from_node) == node_set_.end()) {
    throw std::logic_error("from_node doesn't exist in node_set_\n");
  }

  if (node_set_.find(to_node) == node_set_.end()) {
    throw std::logic_error("to_node doesn't exist in node_set_\n");
  }

  succ_map_.at(from_node).emplace_back(to_node);
  pred_map_.at(to_node).emplace_back(from_node);
}

template <class NodeType>
Graph<NodeType> Graph<NodeType>::transpose() const {
  Graph transpose_graph = copy_and_clear();

  // Flip edges
  for (const auto & node : succ_map_) {
    for (const auto & neighbor : node.second) {
      // The original graph has an edge from node to neighbor, flip that around
      transpose_graph.succ_map_.at(neighbor).emplace_back(node.first);
      transpose_graph.pred_map_.at(node.first).emplace_back(neighbor);
    }
  }
  return transpose_graph;
}

template <class NodeType>
Graph<NodeType> Graph<NodeType>::dominator_tree(const NodeType & start_node) const {
  Graph dominator_tree = copy_and_clear();

  // Connect idom(n) to n
  auto dominators = get_dominators(start_node);
  for (const auto & node : node_set_ - std::set<NodeType>{start_node}) {
    auto idom = get_idom(node, dominators);
    dominator_tree.succ_map_.at(idom).emplace_back(node);
    dominator_tree.pred_map_.at(node).emplace_back(idom);
  }
  return dominator_tree;
}

template <class NodeType>
typename Graph<NodeType>::Dominators Graph<NodeType>::dominance_frontier(const NodeType & start_node) const {
  // TODO: Maybe use a different typedef for dominance_frontier
  Dominators  dominance_frontier;
  const auto dominator_tree = this->dominator_tree(start_node);

  for (const auto & node : node_set_) {
    dominance_frontier[node] = dom_frontier_helper(node, dominator_tree, start_node);
  }
  return dominance_frontier;
}


template <class NodeType>
std::set<NodeType> Graph<NodeType>::dom_frontier_helper(const NodeType & node, const Graph<NodeType> & dominator_tree, const NodeType & start_node) const {
  // Memoize results
  static Dominators dominance_frontier;
  if (dominance_frontier.find(node) != dominance_frontier.end()) {
    return dominance_frontier.at(node);
  }

  std::set<NodeType> S;
  for (const auto & y : succ_map_.at(node)) {
    assert(dominator_tree.pred_map_.at(y).size() == 1);
    if (dominator_tree.pred_map_.at(y).front() != node) {
      S = S + std::set<NodeType>{y};
    }
  }

  for (const auto & child : dominator_tree.succ_map_.at(node)) {
    const auto frontier_child = dom_frontier_helper(child, dominator_tree, start_node);
    for (const auto & w : frontier_child) {
      // if: w's set of dominators does not contain node
      // if: w is node
      const auto dom_set = get_dominators(start_node).at(w);
      if ((dom_set.find(node) == dom_set.end()) or
          (node == w)) {
        S = S + std::set<NodeType>{w};
      }
    }
  }
  // If we get here, the results were not memoized to begin with
  assert(dominance_frontier.find(node) == dominance_frontier.end());
  dominance_frontier[node] = S;
  return S;
}

template <class NodeType>
Graph<NodeType> Graph<NodeType>::copy_and_clear() const {
  Graph copy(*this);
  // Clear out succ_map_ and pred_map_
  for (const auto & node : copy.node_set_) {
    copy.succ_map_.at(node).clear();
    copy.pred_map_.at(node).clear();
  }
  return copy;
}

template <class NodeType>
typename Graph<NodeType>::Dominators Graph<NodeType>::get_dominators(const NodeType & start_node) const {
  // Initialize dominators
  Dominators dominators;
  dominators[start_node] = {start_node};
  for (const auto & node : (node_set_ - std::set<NodeType>{start_node})) {
    dominators[node] = node_set_;
  }

  Dominators prev_dominators;
  while (dominators != prev_dominators) {
    // Save old dominators
    prev_dominators = dominators;

    // Run dataflow equations
    for (const auto & node : (node_set_ - std::set<NodeType>{start_node})) {
      std::set<NodeType> pred_intersection = node_set_;
      for (const auto & pred : pred_map_.at(node)) {
        pred_intersection = pred_intersection * dominators.at(pred);
      }
      dominators.at(node) = std::set<NodeType>{node} + pred_intersection;
    }
  }

  return dominators;
}

template <class NodeType>
void Graph<NodeType>::print_dominators(const Dominators & dominators) const {
  for (const auto & node : dominators) {
    std::cout << node.first << " dominated by ";
    for (const auto & dom_node : node.second) {
      std::cout << dom_node << " ";
    }
    std::cout << "\n";
  }
}

template <class NodeType>
NodeType Graph<NodeType>::get_idom(const NodeType & node, const Graph<NodeType>::Dominators & dominators) const {
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
