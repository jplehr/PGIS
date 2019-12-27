#include "gtest/gtest.h"

#include "Callgraph.h"

TEST(CallgraphBasics, EmptyCG) {
  Callgraph c;
  ASSERT_EQ(nullptr, c.findMain());
  ASSERT_EQ(0, c.size());
}

TEST(CallgraphBasics, OnlyMainCG) {
  Callgraph c;
  ASSERT_EQ(nullptr, c.findMain());
  auto n = std::make_shared<CgNode>("main");
  c.insert(n);
  ASSERT_EQ(n, c.findMain());
  ASSERT_EQ(n, c.findNode("main"));
  ASSERT_EQ(n, *(c.begin()));
  ASSERT_EQ(true, c.hasNode("main"));
  ASSERT_EQ(1, c.size());
}

TEST(CallgraphBasics, TwoNodeConnectedCG) {
  Callgraph c;
  auto main = std::make_shared<CgNode>("main");
  auto child = std::make_shared<CgNode>("child");
  main->addChildNode(child);
  child->addParentNode(main);
  c.insert(main);
  c.insert(child);
  ASSERT_EQ(2, c.size());
  ASSERT_EQ(true, c.hasNode("main"));
  ASSERT_EQ(true, c.hasNode("child"));
  ASSERT_EQ(main, c.findMain());
  auto founMain = c.findMain();
  ASSERT_EQ(child, (*founMain->getChildNodes().begin()));
}

TEST(CallgraphBasics, HasNodeGetLastSearchedTest) {
  Callgraph c;
  auto main = std::make_shared<CgNode>("child");
  c.insert(main);
  ASSERT_EQ(nullptr, c.getLastSearched());
  c.hasNode("child");
  ASSERT_EQ(main, c.getLastSearched());
}

TEST(CallgraphBasics, InsertTwiceTest) {
  Callgraph c;
  auto node1 = std::make_shared<CgNode>("node");
  auto node2 = std::make_shared<CgNode>("node");
  c.insert(node1);
  c.insert(node2);
  ASSERT_EQ(1, c.size());
}

TEST(CallrgaphBasics, SearchNodes) {
  Callgraph c;
  auto node = std::make_shared<CgNode>("node1");
  c.insert(node);
  auto node2 = std::make_shared<CgNode>("node2");
  node2->addChildNode(node);
  node->addParentNode(node2);
  c.insert(node2);
  ASSERT_EQ(nullptr, c.findMain());
  ASSERT_EQ(false, c.hasNode("main"));
  ASSERT_EQ(false, c.hasNode("nodeee"));
  ASSERT_EQ(nullptr, c.getLastSearched());
}