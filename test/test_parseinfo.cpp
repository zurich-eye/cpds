#include <gtest/gtest.h>
#include "cpds/parseinfo.hpp"
#include "cpds/node.hpp"

using namespace cpds;

TEST(ParseMark, Default)
{
  ParseMark p1;
  ParseMark p2(2, 3);
  ParseMark p3(std::make_shared<std::string>("testfile"), 4, 5);

  EXPECT_EQ("<unknown>", p1.filename());
  EXPECT_EQ(-1, p1.line());
  EXPECT_EQ(-1, p1.position());

  EXPECT_EQ("<unknown>", p2.filename());
  EXPECT_EQ(2, p2.line());
  EXPECT_EQ(3, p2.position());

  EXPECT_EQ("testfile", p3.filename());
  EXPECT_EQ(4, p3.line());
  EXPECT_EQ(5, p3.position());
}

TEST(ParseInfo, Default)
{
  Node n1;
  ParseInfo pi;
  pi[n1.id()] = ParseMark(6, 7);
  pi[n1.id()+1] = ParseMark(8, 0);

  EXPECT_TRUE(pi.hasMark(n1));
  EXPECT_TRUE(pi.hasMark(n1.id()+1));
  EXPECT_FALSE(pi.hasMark(n1.id()+2));

  EXPECT_EQ(6, pi.getMark(n1).line());
  EXPECT_EQ(7, pi.getMark(n1).position());
  EXPECT_EQ(8, pi.getMark(n1.id()+1).line());
  EXPECT_EQ(0, pi.getMark(n1.id()+1).position());

  EXPECT_THROW(pi.getMark(n1.id()+2), std::out_of_range);
}
