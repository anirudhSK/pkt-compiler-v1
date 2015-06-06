AM_CXXFLAGS = $(PICKY_CXXFLAGS) -isystem $(srcdir)/../third_party/ -pthread -I $(srcdir)/..
LDADD = -lm $(srcdir)/../third_party/gtest/libgtest.la $(srcdir)/../libjayhawk.la

# Define unit tests
gtest_main_source = main.cc
TESTS = flipped_cfg dominator_tree dominator_tree_hard dominator_tree_medium
check_PROGRAMS = $(TESTS)

flipped_cfg_SOURCES = $(gtest_main_source) flipped_cfg.cc
dominator_tree_SOURCES = $(gtest_main_source) dominator_tree.cc
dominator_tree_hard_SOURCES = $(gtest_main_source) dominator_tree_hard.cc
dominator_tree_medium_SOURCES = $(gtest_main_source) dominator_tree_medium.cc
