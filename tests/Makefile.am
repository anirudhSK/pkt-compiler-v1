AM_CXXFLAGS = $(PICKY_CXXFLAGS) -isystem $(srcdir)/../third_party/ -pthread -I $(srcdir)/..
LDADD = -lm $(srcdir)/../third_party/gtest/libgtest.la $(srcdir)/../libjayhawk.la

# Define unit tests
gtest_main_source = main.cc
check_PROGRAMS = flipped_cfg dominator_tree dominator_tree_hard dominator_tree_medium dominance_frontier post_dominance_frontiers control_dependence_graph
TESTS = $(check_PROGRAMS) phi.sh

flipped_cfg_SOURCES = $(gtest_main_source) flipped_cfg.cc
dominator_tree_SOURCES = $(gtest_main_source) dominator_tree.cc
dominator_tree_hard_SOURCES = $(gtest_main_source) dominator_tree_hard.cc
dominator_tree_medium_SOURCES = $(gtest_main_source) dominator_tree_medium.cc
dominance_frontier_SOURCES = $(gtest_main_source) dominance_frontier.cc
post_dominance_frontiers_SOURCES = $(gtest_main_source) post_dominance_frontiers.cc
control_dependence_graph_SOURCES = $(gtest_main_source) control_dependence_graph.cc

# LLVM tests using opt
EXTRA_DIST = phi.sh
