#ifndef GRAPH_H_
#define GRAPH_H_

#include <map>
#include <set>
#include <vector>
#include <ostream>

/// Adjacency list representation of graph
/// Store both out and in edges
template <class NodeType>
class Graph {
 public:
  /// Add node alone to existing graph, check that node doesn't already exist
  void add_node(const NodeType & node);

  /// Add edge to existing graph, check that both from_node and to_node exist
  void add_edge(const NodeType & from_node, const NodeType & to_node);

  /// Find the graph transpose G', i.e. for every edge u-->v in G,
  /// there is an edge v-->u in G'
  /// Returning a Graph is ok because of C++11's move semantics
  /// and the likely move elision from the compiler
  Graph<NodeType> transpose() const;

  /// Copy over graph and clear out all edges
  Graph<NodeType> copy_and_clear() const;

  /// Print graph to stream
  friend std::ostream & operator<< (std::ostream & out, const Graph<NodeType> & graph) {
    for (const auto & node : graph.succ_map_) {
      if (not node.second.empty()) {
        out << node.first;
        out << " ---> ";
        for (const auto & neighbor : node.second) {
          out << neighbor;
          out << " ";
        }
        out << "\n";
      }
    }
    return out;
  }

  bool operator==(const Graph<NodeType> & b) {
    return (this->node_set_ == b.node_set_) and
           (this->succ_map_ == b.succ_map_) and
           (this->pred_map_ == b.pred_map_);
  }

  /// Accessors
  const std::set<NodeType> & node_set() const { return node_set_; }
  const auto & succ_map() const { return succ_map_; }
  const auto & pred_map() const { return pred_map_; }

 private:
  /// Set of all nodes in the graph
  std::set<NodeType> node_set_ = {};

  /// Successor map from a node to all successor nodes (outgoing edges)
  std::map<NodeType, std::vector<NodeType>> succ_map_ = {};

  /// Predecessor map from a node to all predecessor nodes (incoming edges)
  std::map<NodeType, std::vector<NodeType>> pred_map_ = {};
};

#endif  // GRAPH_H_
