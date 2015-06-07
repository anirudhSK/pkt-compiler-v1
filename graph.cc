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
Graph<NodeType> Graph<NodeType>::copy_and_clear() const {
  Graph copy(*this);
  // Clear out succ_map_ and pred_map_
  for (const auto & node : copy.node_set_) {
    copy.succ_map_.at(node).clear();
    copy.pred_map_.at(node).clear();
  }
  return copy;
}
