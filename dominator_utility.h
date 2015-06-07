#ifndef DOMINATOR_UTILITY_H_
#define DOMINATOR_UTILITY_H_

#include "graph.h"

/// Utility class to compute dominator tree and dominance frontiers
/// given a flow graph (a graph augmented with a start node)
template <class NodeType>
class DominatorUtility {
 public:
  /// Map from a node to set of nodes
  /// Used both to store the set of ALL dominators for a node and
  /// the dominance frontier for each node
  typedef std::map<NodeType, std::set<NodeType>> NodeSetMap;

  /// Convenience typedef for set of nodes
  typedef std::set<NodeType> NodeSet;

  /// Delete copy constructor to shut up effc++
  DominatorUtility(const DominatorUtility<NodeType> &) = delete;

  /// Delete copy assignment to shut up effc++
  DominatorUtility & operator=(const DominatorUtility<NodeType> &) = delete;

  /// Constructor for DominatorUtility from Graph object and start node
  DominatorUtility(const Graph<NodeType> & t_graph, const NodeType & t_start_node);

  /// Return dominator tree
  auto dominator_tree() const { return dominator_tree_; };

  /// Return dominance frontier for all nodes
  auto dominance_frontier() const { return dominance_frontier_; };

  /// Routine to print out dominators
  void print_dominators() const;

 private:
  /// Compute dominators for each node using naive dataflow equations
  /// (Algorithm 430: Immediate Predominators in a Directed Graph)
  /// http://en.wikipedia.org/wiki/Dominator_%28graph_theory%29#Algorithms
  static auto construct_dominators(const Graph<NodeType> & t_graph,
                                   const NodeType & t_start_node);

  /// Construct dom tree use dominators and get_idom
  /// Dom tree connects every node to its idom
  static auto construct_dom_tree(const Graph<NodeType> & t_graph,
                                 const NodeType & t_start_node,
                                 const NodeSetMap & t_dominators);

  /// Get immediate dominator for each node
  /// Naive implementation from Page 380 of Appel's book
  static auto get_idom(const NodeType & node, const NodeSetMap & dominators);

  /// Helper to compute dominance frontier for one node (Page 406 of Appel's book)
  static NodeSet dom_frontier_helper(const NodeType & node,
                                     const Graph<NodeType> & t_graph,
                                     const Graph<NodeType> & t_dom_tree,
                                     const NodeSetMap & t_dominators);

  /// Compute dominance frontiers for all nodes by calling
  /// dom_frontier_helper on each node
  static auto construct_dom_frontiers(const Graph<NodeType> & t_graph,
                                      const Graph<NodeType> & t_dom_tree,
                                      const NodeSetMap & t_dominators);

  /// Underlying graph for which we are computing dominator tree
  const Graph<NodeType> graph_;

  /// Start node for computing dominator tree
  const NodeType start_node_;

  /// Set of dominators for each node in the graph
  const NodeSetMap dominators_; 

  /// Dominator Tree itself
  const Graph<NodeType> dominator_tree_;

  /// Dominance frontier for each node in the graph
  const NodeSetMap dominance_frontier_;
};

#endif  // DOMINATOR_UTILITY_H_
