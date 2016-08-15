#include <sstream>
#include <gtest/gtest.h>
#include "cpds/exception.hpp"
#include "cpds/node.hpp"

using namespace cpds;

// enforce local linkage
namespace {

TEST(Exception, Default)
{
  Node n;
  StringPtr fn = std::make_shared<String>("file");
  Exception e1("test");
  Exception e2("t2", n);
  Exception e3("t3", fn, 4, 6);

  EXPECT_EQ(String("test"), String(e1.what()));
  EXPECT_EQ(std::numeric_limits<uint32_t>::max(), e1.nodeId());
  EXPECT_FALSE(e1.hasParseMark());
  EXPECT_EQ("<unknown>", e1.filename());
  EXPECT_EQ(-1, e1.line());
  EXPECT_EQ(-1, e1.position());

  EXPECT_EQ(String("t2"), String(e2.what()));
  EXPECT_EQ(n.id(), e2.nodeId());
  EXPECT_FALSE(e2.hasParseMark());
  EXPECT_EQ("<unknown>", e2.filename());
  EXPECT_EQ(-1, e2.line());
  EXPECT_EQ(-1, e2.position());

  EXPECT_EQ(String("t3"), String(e3.what()));
  EXPECT_EQ(std::numeric_limits<uint32_t>::max(), e3.nodeId());
  EXPECT_TRUE(e3.hasParseMark());
  EXPECT_EQ("file", e3.filename());
  EXPECT_EQ(4, e3.line());
  EXPECT_EQ(6, e3.position());

  std::stringstream strm1;
  std::stringstream strm2;

  strm1 << e1;
  strm2 << e3;

  EXPECT_EQ("test", strm1.str());
  EXPECT_EQ("t3, file 'file', line 4, position 6", strm2.str());

  e1.setParseMark(ParseMark(fn, 2, 7));

  EXPECT_EQ("file", e1.filename());
  EXPECT_EQ(2, e1.line());
  EXPECT_EQ(7, e1.position());
}

TEST(Exception, OtherExceptions)
{
  Node n;

  TypeException te;
  EXPECT_EQ("data type mismatch", te.message());

  te = TypeException(n);
  EXPECT_EQ("data type mismatch", te.message());
  EXPECT_EQ(n.id(), te.nodeId());

  OverflowException oe;
  EXPECT_EQ("narrowing from unsigned to signed generates overflow",
            oe.message());

  KeyException ke;
  EXPECT_EQ("key not found in sequence or map", ke.message());

  ImportException ie(std::make_shared<String>("tf"), 3, 5);
  EXPECT_EQ("invalid data format", ie.message());
  EXPECT_EQ("tf", ie.filename());
  EXPECT_EQ(3, ie.line());
  EXPECT_EQ(5, ie.position());

  ie = ImportException("tm", std::make_shared<String>("ff"), 7, 8);
  EXPECT_EQ("tm", ie.message());
  EXPECT_EQ("ff", ie.filename());
  EXPECT_EQ(7, ie.line());
  EXPECT_EQ(8, ie.position());

  ValidationException ve("error", n);
  EXPECT_EQ("error", ve.message());
  EXPECT_EQ(n.id(), ve.nodeId());

  IntRangeException ire(4, 8, 12, n);
  EXPECT_EQ("value out of bounds: range [4:8], actual 12", ire.message());
  EXPECT_EQ(n.id(), ire.nodeId());

  FloatRangeException fre(-1.5, 1.5, -2.0, n);
  EXPECT_EQ("value out of bounds: range [-1.500000:1.500000], actual -2.000000",
            fre.message());
  EXPECT_EQ(n.id(), fre.nodeId());
}

} // unnamed namespace
