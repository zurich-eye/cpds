#include <gtest/gtest.h>
#include "cpds/parseinfo.hpp"

using namespace cpds;

TEST(ParseInfo, Default)
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
