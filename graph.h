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
  /// Dominator sets for all nodes in the graph
  typedef std::map<NodeType, std::set<NodeType>> DominatorSets;

  /// Add node alone to existing graph, check that node doesn't already exist
  void add_node(const NodeType & node);

  /// Add edge to existing graph, check that both from_node and to_node exist
  void add_edge(const NodeType & from_node, const NodeType & to_node);

  /// Find the graph transpose G', i.e. for every edge u-->v in G,
  /// there is an edge v-->u in G'
  /// Returning a Graph is ok because of C++11's move semantics
  /// and the likely move elision from the compiler
  Graph transpose() const;

  /// Compute dominator sets for each node using naive dataflow equations
  /// (Algorithm 430: Immediate Predominators in a Directed Graph)
  /// http://en.wikipedia.org/wiki/Dominator_%28graph_theory%29#Algorithms
  DominatorSets get_dominator_sets(const NodeType & start_node) const;

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

 private:
  /// Set of all nodes in the graph
  std::set<NodeType> node_set_ = {};

  /// Successor map from a node to all successor nodes (outgoing edges)
  std::map<NodeType, std::vector<NodeType>> succ_map_ = {};

  /// Predecessor map from a node to all predecessor nodes (incoming edges)
  std::map<NodeType, std::vector<NodeType>> pred_map_ = {};
};

#endif  // GRAPH_H_
