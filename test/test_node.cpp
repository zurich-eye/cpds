#include <gtest/gtest.h>
#include "cpds/node.hpp"
#include "cpds/exception.hpp"

using namespace cpds;

// the template must be in the cpds namespace
namespace cpds {

struct TestStruct {
  bool a_ = true;
  double b_ = 44.5;
  std::string c_ = "test";
}; // struct TestStruct

template<>
struct custom_converter<TestStruct> {

  static Node transform(const TestStruct& s)
  {
    return Sequence({s.a_, s.b_, s.c_});
  }

  static TestStruct transform(const Node& node)
  {
    TestStruct s;
    s.a_ = node[0].boolValue();
    s.b_ = node[1].floatValue();
    s.c_ = node[2].stringValue();
    return s;
  }

}; // struct custom_converter<TestStruct>

} // namespace cpds

TEST(Node, Null)
{
  Node node;

  EXPECT_EQ(NodeType::Null, node.type());
  EXPECT_TRUE(node.isNull());
  EXPECT_FALSE(node.isBool());
  EXPECT_FALSE(node.isInt());
  EXPECT_FALSE(node.isFloat());
  EXPECT_FALSE(node.isString());
  EXPECT_FALSE(node.isSequence());
  EXPECT_FALSE(node.isMap());

  EXPECT_THROW(node.boolValue(), TypeException);
  EXPECT_THROW(node.intValue(), TypeException);
  EXPECT_THROW(node.floatValue(), TypeException);
  EXPECT_THROW(node.stringValue(), TypeException);

  EXPECT_FALSE(node.asBool());
  EXPECT_EQ(0, node.asInt());
  EXPECT_DOUBLE_EQ(0.0, node.asFloat());

  EXPECT_EQ(0, node.size());

  EXPECT_THROW(node[0], TypeException);
  EXPECT_THROW(node["test"], TypeException);
}

TEST(Node, Bool)
{
  Node node = false;

  EXPECT_EQ(NodeType::Boolean, node.type());
  EXPECT_FALSE(node.isNull());
  EXPECT_TRUE(node.isBool());
  EXPECT_FALSE(node.isInt());
  EXPECT_FALSE(node.isFloat());
  EXPECT_FALSE(node.isString());
  EXPECT_FALSE(node.isSequence());
  EXPECT_FALSE(node.isMap());

  EXPECT_FALSE(node.boolValue());
  EXPECT_THROW(node.intValue(), TypeException);
  EXPECT_THROW(node.floatValue(), TypeException);
  EXPECT_THROW(node.stringValue(), TypeException);

  EXPECT_FALSE(node.asBool());
  EXPECT_EQ(0, node.asInt());
  EXPECT_DOUBLE_EQ(0.0, node.asFloat());

  EXPECT_EQ(0, node.size());

  EXPECT_THROW(node[0], TypeException);
  EXPECT_THROW(node["test"], TypeException);
}

TEST(Node, Int)
{
  Node node = -4;

  EXPECT_EQ(NodeType::Integer, node.type());
  EXPECT_FALSE(node.isNull());
  EXPECT_FALSE(node.isBool());
  EXPECT_TRUE(node.isInt());
  EXPECT_FALSE(node.isFloat());
  EXPECT_FALSE(node.isString());
  EXPECT_FALSE(node.isSequence());
  EXPECT_FALSE(node.isMap());

  EXPECT_THROW(node.boolValue(), TypeException);
  EXPECT_EQ(-4, node.intValue());
  EXPECT_THROW(node.floatValue(), TypeException);
  EXPECT_THROW(node.stringValue(), TypeException);

  EXPECT_TRUE(node.asBool());
  EXPECT_EQ(-4, node.asInt());
  EXPECT_DOUBLE_EQ(-4.0, node.asFloat());

  EXPECT_EQ(0, node.size());

  EXPECT_THROW(node[0], TypeException);
  EXPECT_THROW(node["test"], TypeException);

  Node node2 = 4u;
  EXPECT_EQ(NodeType::Integer, node2.type());
  EXPECT_EQ(4, node2.intValue());

  Node node3 = -25l;
  EXPECT_EQ(NodeType::Integer, node3.type());
  EXPECT_EQ(-25, node3.intValue());

  Node node4 = 35ul;
  EXPECT_EQ(NodeType::Integer, node4.type());
  EXPECT_EQ(35, node4.intValue());

  Node node5 = -100ll;
  EXPECT_EQ(NodeType::Integer, node5.type());
  EXPECT_EQ(-100, node5.intValue());

  Node node6 = 1000ull;
  EXPECT_EQ(NodeType::Integer, node6.type());
  EXPECT_EQ(1000, node6.intValue());
}

TEST(Node, Float)
{
  Node node = 5.6;

  EXPECT_EQ(NodeType::FloatingPoint, node.type());
  EXPECT_FALSE(node.isNull());
  EXPECT_FALSE(node.isBool());
  EXPECT_FALSE(node.isInt());
  EXPECT_TRUE(node.isFloat());
  EXPECT_FALSE(node.isString());
  EXPECT_FALSE(node.isSequence());
  EXPECT_FALSE(node.isMap());

  EXPECT_THROW(node.boolValue(), TypeException);
  EXPECT_THROW(node.intValue(), TypeException);
  EXPECT_DOUBLE_EQ(5.6, node.floatValue());
  EXPECT_THROW(node.stringValue(), TypeException);

  EXPECT_TRUE(node.asBool());
  EXPECT_EQ(5, node.asInt());
  EXPECT_DOUBLE_EQ(5.6, node.asFloat());

  EXPECT_EQ(0, node.size());

  EXPECT_THROW(node[0], TypeException);
  EXPECT_THROW(node["test"], TypeException);
}

TEST(Node, String)
{
  Node node = "test";

  EXPECT_EQ(NodeType::String, node.type());
  EXPECT_FALSE(node.isNull());
  EXPECT_FALSE(node.isBool());
  EXPECT_FALSE(node.isInt());
  EXPECT_FALSE(node.isFloat());
  EXPECT_TRUE(node.isString());
  EXPECT_FALSE(node.isSequence());
  EXPECT_FALSE(node.isMap());

  EXPECT_THROW(node.boolValue(), TypeException);
  EXPECT_THROW(node.intValue(), TypeException);
  EXPECT_THROW(node.floatValue(), TypeException);
  EXPECT_EQ("test", node.stringValue());

  EXPECT_THROW(node.asBool(), TypeException);
  EXPECT_THROW(node.asInt(), TypeException);
  EXPECT_THROW(node.asFloat(), TypeException);

  EXPECT_EQ(0, node.size());

  EXPECT_THROW(node[0], TypeException);
  EXPECT_THROW(node["test"], TypeException);
}

TEST(Node, Sequence)
{
  Node node = Sequence({ true, 5});

  EXPECT_EQ(NodeType::Sequence, node.type());

  EXPECT_FALSE(node.isNull());
  EXPECT_FALSE(node.isBool());
  EXPECT_FALSE(node.isInt());
  EXPECT_FALSE(node.isFloat());
  EXPECT_FALSE(node.isString());
  EXPECT_TRUE(node.isSequence());
  EXPECT_FALSE(node.isMap());

  EXPECT_THROW(node.boolValue(), TypeException);
  EXPECT_THROW(node.intValue(), TypeException);
  EXPECT_THROW(node.floatValue(), TypeException);
  EXPECT_THROW(node.stringValue(), TypeException);

  EXPECT_THROW(node.asBool(), TypeException);
  EXPECT_THROW(node.asInt(), TypeException);
  EXPECT_THROW(node.asFloat(), TypeException);

  EXPECT_EQ(2, node.size());

  EXPECT_TRUE(node[0].boolValue());
  EXPECT_EQ(5, node[1].intValue());
  EXPECT_THROW(node[2], KeyException);
  EXPECT_TRUE(node.sequence()[0].boolValue());
  EXPECT_EQ(5, node.sequence()[1].intValue());

  EXPECT_THROW(node["test"], TypeException);
  EXPECT_THROW(node.find("test"), TypeException);
  EXPECT_THROW(node.end(), TypeException);
  EXPECT_THROW(node.erase("test"), TypeException);
}

TEST(Node, Map)
{
  Node node = Map({ {"z", 1}, {"b",  true}, {"a", 4.3} });

  EXPECT_EQ(NodeType::Map, node.type());

  EXPECT_FALSE(node.isNull());
  EXPECT_FALSE(node.isBool());
  EXPECT_FALSE(node.isInt());
  EXPECT_FALSE(node.isFloat());
  EXPECT_FALSE(node.isString());
  EXPECT_FALSE(node.isSequence());
  EXPECT_TRUE(node.isMap());

  EXPECT_THROW(node.boolValue(), TypeException);
  EXPECT_THROW(node.intValue(), TypeException);
  EXPECT_THROW(node.floatValue(), TypeException);
  EXPECT_THROW(node.stringValue(), TypeException);

  EXPECT_THROW(node.asBool(), TypeException);
  EXPECT_THROW(node.asInt(), TypeException);
  EXPECT_THROW(node.asFloat(), TypeException);

  EXPECT_EQ(3, node.size());

  EXPECT_THROW(node[0], TypeException);
  EXPECT_THROW(node.sequence(), TypeException);

  EXPECT_EQ(1, node["z"].intValue());
  EXPECT_TRUE(node["b"].boolValue());
  EXPECT_DOUBLE_EQ(4.3, node["a"].floatValue());
  EXPECT_EQ(Node(), node["f"]);
  EXPECT_EQ(Node(), node["zz"]);

  EXPECT_EQ(5, node.size()); // elements "f" and "zz" were added

  // test that the underlying data was not modified with the inserts
  EXPECT_EQ(1, node["z"].intValue());
  EXPECT_TRUE(node["b"].boolValue());
  EXPECT_DOUBLE_EQ(4.3, node["a"].floatValue());

  EXPECT_EQ(node.end(), node.find("y"));
  EXPECT_EQ(4.3, node.find("a")->second.floatValue());

  node.erase("b");
  EXPECT_EQ(4, node.size());
  EXPECT_FALSE(node.find("a") == node.end());
  EXPECT_TRUE(node.find("b") == node.end());
  EXPECT_FALSE(node.find("f") == node.end());
  EXPECT_FALSE(node.find("z") == node.end());
  EXPECT_FALSE(node.find("zz") == node.end());
}

TEST(Node, CustomStruct)
{
  Node node = TestStruct();

  EXPECT_TRUE(node.isSequence());
  EXPECT_TRUE(node[0].boolValue());
  EXPECT_DOUBLE_EQ(44.5, node[1].floatValue());
  EXPECT_EQ("test", node[2].stringValue());

  node = Sequence({false, 0.001, "other"});

  TestStruct s = node.as<TestStruct>();
  EXPECT_FALSE(s.a_);
  EXPECT_DOUBLE_EQ(0.001, s.b_);
  EXPECT_EQ("other", s.c_);
}

TEST(Node, CopyAndMoveSemantics)
{
  const Node* sp1;
  const Node* sp2;

  Node n1 = 5;
  Node n2 = 6.7;
  Node n3 = Sequence({1, 2, 3});
  sp1 = n3.sequence().data();

  Node n4(n1); // copy constructor
  EXPECT_EQ(NodeType::Integer, n4.type());
  EXPECT_EQ(5, n4.intValue());

  Node n5(std::move(n3)); // move constructor
  EXPECT_EQ(NodeType::Sequence, n5.type());
  EXPECT_EQ(NodeType::Null, n3.type());
  EXPECT_EQ(Sequence({1, 2, 3}), n5.sequence());
  sp2 = n5.sequence().data();
  EXPECT_EQ(sp1, sp2); // the pointer was moved

  n3 = n2; // copy assignment
  EXPECT_EQ(NodeType::FloatingPoint, n3.type());
  EXPECT_EQ(6.7, n3.floatValue());

  n2 = std::move(n5); // move assignment
  EXPECT_EQ(NodeType::Sequence, n2.type());
  EXPECT_EQ(NodeType::FloatingPoint, n5.type()); // there was a swap
  EXPECT_EQ(Sequence({1, 2, 3}), n2.sequence());
  sp2 = n2.sequence().data();
  EXPECT_EQ(sp1, sp2);
}

TEST(Node, Comparison)
{
  Node n1 = 5;
  Node n2 = "test";
  Node n3 = "abcd";
  Node n4 = "test";

  EXPECT_TRUE(n1 == n1);
  EXPECT_FALSE(n1 == n2);
  EXPECT_FALSE(n1 == n3);
  EXPECT_FALSE(n1 == n4);
  EXPECT_FALSE(n1 != n1);
  EXPECT_TRUE(n1 != n2);
  EXPECT_TRUE(n1 != n3);
  EXPECT_TRUE(n1 != n4);

  EXPECT_FALSE(n2 == n1);
  EXPECT_TRUE(n2 == n2);
  EXPECT_FALSE(n2 == n3);
  EXPECT_TRUE(n2 == n4);
  EXPECT_TRUE(n2 != n1);
  EXPECT_FALSE(n2 != n2);
  EXPECT_TRUE(n2 != n3);
  EXPECT_FALSE(n2 != n4);

  EXPECT_FALSE(n3 == n1);
  EXPECT_FALSE(n3 == n2);
  EXPECT_TRUE(n3 == n3);
  EXPECT_FALSE(n3 == n4);
  EXPECT_TRUE(n3 != n1);
  EXPECT_TRUE(n3 != n2);
  EXPECT_FALSE(n3 != n3);
  EXPECT_TRUE(n3 != n4);

  EXPECT_FALSE(n4 == n1);
  EXPECT_TRUE(n4 == n2);
  EXPECT_FALSE(n4 == n3);
  EXPECT_TRUE(n4 == n4);
  EXPECT_TRUE(n4 != n1);
  EXPECT_FALSE(n4 != n2);
  EXPECT_TRUE(n4 != n3);
  EXPECT_FALSE(n4 != n4);
}