#include <gtest/gtest.h>
#include "cpds/node.hpp"
#include "cpds/validator.hpp"
#include "cpds/exception.hpp"

using namespace cpds;

namespace {

void evenInt(const Node& node, const Validator& validator)
{
  Int val = node.intValue();
  if (val % 2 != 0)
  {
    throw ValidationException("number is not even", node);
  }
}

void oddInt(const Node& node, const Validator& validator)
{
  Int val = node.intValue();
  if (val % 2 == 0)
  {
    throw ValidationException("number is not odd", node);
  }
}

void positiveFloat(const Node& node, const Validator& validator)
{
  Float val = node.floatValue();
  if (val < 0.0)
  {
    throw ValidationException("value is negative", node);
  }
}

void negativeFloat(const Node& node, const Validator& validator)
{
  Float val = node.floatValue();
  if (val > 0.0)
  {
    throw ValidationException("value is positive", node);
  }
}

void isEmptyString(const Node& node, const Validator& validator)
{
  const String& val = node.stringValue();
  if (!val.empty())
  {
    throw ValidationException("string not empty", node);
  }
}

void isNonEmptyString(const Node& node, const Validator& validator)
{
  const String& val = node.stringValue();
  if (val.empty())
  {
    throw ValidationException("string empty", node);
  }
}

void threeIntegers(const Node& node, const Validator& /*validator*/)
{
  const Sequence& seq = node.sequence();

  if (seq.size() != 3)
  {
    throw ValidationException("wrong size", node);
  }
  for (const Node& child : seq)
  {
    if (child.type() != NodeType::Integer)
    {
      throw ValidationException("wrong child type", child);
    }
  }
}

bool hasKeyB(const Node& node)
{
  if (node.find("b") != node.end())
  {
    return true;
  }
  return false;
}

} // unnamed namespace

TEST(Validator, CopyAndMoveSemantics)
{
  // custom validation function
  {
    Validator v0 = NullType();
    Validator v1 = StringType(evenInt);
    Validator v2(v1);
    EXPECT_EQ(NodeType::String, v2.type());
    EXPECT_TRUE(evenInt == v2.validationFcn());

    Validator v3(std::move(v2));
    EXPECT_EQ(NodeType::String, v3.type());
    EXPECT_TRUE(evenInt == v3.validationFcn());

    v0 = v3;
    EXPECT_EQ(NodeType::String, v0.type());
    EXPECT_TRUE(evenInt == v0.validationFcn());

    v2 = std::move(v0);
    EXPECT_EQ(NodeType::String, v2.type());
    EXPECT_TRUE(evenInt == v2.validationFcn());
  }

  // integer range
  {
    Validator v1 = IntegerType(0, 3);
    Validator v2(v1);

    EXPECT_EQ(IntRange(0, 3), v2.intRange());
    EXPECT_NE(&v1.intRange(), &v2.intRange()); // a copy was made
    const IntRange* p2 = &v2.intRange();

    Validator v3(std::move(v2));
    EXPECT_EQ(p2, &v3.intRange()); // the data was moved
    EXPECT_THROW(v2.intRange(), TypeException); // the data was moved

    v2 = v3; // copy assignment
    EXPECT_EQ(IntRange(0, 3), v2.intRange());
    EXPECT_NE(p2, &v2.intRange()); // the data was copied

    v2 = std::move(v3);
    EXPECT_EQ(IntRange(0, 3), v2.intRange());
    EXPECT_EQ(p2, &v2.intRange()); // the data was moved
  }

  // floating point range
  {
    Validator v1 = FloatingPointType(0.0, 3.0);
    Validator v2(v1);

    EXPECT_EQ(FloatRange(0.0, 3.0), v2.floatRange());
    EXPECT_NE(&v1.floatRange(), &v2.floatRange()); // a copy was made
    const FloatRange* p2 = &v2.floatRange();

    Validator v3(std::move(v2));
    EXPECT_EQ(p2, &v3.floatRange()); // the data was moved
    EXPECT_THROW(v2.floatRange(), TypeException); // the data was moved

    v2 = v3; // copy assignment
    EXPECT_EQ(FloatRange(0.0, 3.0), v2.floatRange());
    EXPECT_NE(p2, &v2.floatRange()); // the data was copied

    v2 = std::move(v3);
    EXPECT_EQ(FloatRange(0.0, 3.0), v2.floatRange());
    EXPECT_EQ(p2, &v2.floatRange()); // the data was moved
  }

  // sequence
  {
    Validator v1 = SequenceType();
    Validator v2(v1);

    EXPECT_NE(&v1.seqValidators(), &v2.seqValidators()); // a copy was made
    const ValidatorVector* p2 = &v2.seqValidators();

    Validator v3(std::move(v2));
    EXPECT_THROW(v2.seqValidators(), TypeException); // the data was moved
    EXPECT_EQ(p2, &v3.seqValidators());

    v2 = v3; // copy assignment
    EXPECT_NE(p2, &v2.seqValidators()); // the data was copied

    v2 = std::move(v3);
    EXPECT_EQ(p2, &v2.seqValidators()); // the data was copied
  }

  // map
  {
    Validator v1 = MapType();
    Validator v2(v1);

    EXPECT_NE(&v1.mapGroups(), &v2.mapGroups()); // a copy was made
    const MapGroupVector* p2 = &v2.mapGroups();

    Validator v3(std::move(v2));
    EXPECT_THROW(v2.mapGroups(), TypeException); // the data was moved
    EXPECT_EQ(p2, &v3.mapGroups());

    v2 = v3; // copy assignment
    EXPECT_NE(p2, &v2.mapGroups()); // the data was copied

    v2 = std::move(v3);
    EXPECT_EQ(p2, &v2.mapGroups()); // the data was copied
  }
}

TEST(Validator, Null)
{
  Node node;

  Validator v1 = NullType();
  Validator v2 = IntegerType();

  EXPECT_NO_THROW(v1.validate(node));
  EXPECT_THROW(v2.validate(node), TypeException);
}

TEST(Validator, Boolean)
{
  Node node = false;

  Validator v1 = BooleanType();
  Validator v2 = IntegerType();

  EXPECT_NO_THROW(v1.validate(node));
  EXPECT_THROW(v2.validate(node), TypeException);
}

TEST(Validator, Integer)
{
  Node node1 = 6;
  Node node2 = 17;

  Validator v1 = IntegerType();
  Validator v2 = IntegerType(0, 10);
  Validator v3 = IntegerType(10, 20);
  Validator v4 = IntegerType(evenInt);
  Validator v5 = IntegerType(oddInt);
  Validator v6 = BooleanType();

  EXPECT_NO_THROW(v1.validate(node1));
  EXPECT_NO_THROW(v2.validate(node1));
  EXPECT_THROW(v3.validate(node1), IntRangeException);
  EXPECT_NO_THROW(v4.validate(node1));
  EXPECT_THROW(v5.validate(node1), ValidationException);
  EXPECT_THROW(v6.validate(node1), TypeException);

  EXPECT_NO_THROW(v1.validate(node2));
  EXPECT_THROW(v2.validate(node2), IntRangeException);
  EXPECT_NO_THROW(v3.validate(node2));
  EXPECT_THROW(v4.validate(node2), ValidationException);
  EXPECT_NO_THROW(v5.validate(node2));
  EXPECT_THROW(v6.validate(node2), TypeException);
}

TEST(Validator, FloatingPoint)
{
  Node node1 = 2.5;
  Node node2 = -4.0;

  Validator v1 = FloatingPointType();
  Validator v2 = FloatingPointType(0.0, 10.0);
  Validator v3 = FloatingPointType(-10.0, -2.0);
  Validator v4 = FloatingPointType(positiveFloat);
  Validator v5 = FloatingPointType(negativeFloat);
  Validator v6 = BooleanType();

  EXPECT_NO_THROW(v1.validate(node1));
  EXPECT_NO_THROW(v2.validate(node1));
  EXPECT_THROW(v3.validate(node1), FloatRangeException);
  EXPECT_NO_THROW(v4.validate(node1));
  EXPECT_THROW(v5.validate(node1), ValidationException);
  EXPECT_THROW(v6.validate(node1), TypeException);

  EXPECT_NO_THROW(v1.validate(node2));
  EXPECT_THROW(v2.validate(node2), FloatRangeException);
  EXPECT_NO_THROW(v3.validate(node2));
  EXPECT_THROW(v4.validate(node2), ValidationException);
  EXPECT_NO_THROW(v5.validate(node2));
  EXPECT_THROW(v6.validate(node2), TypeException);
}

TEST(Validator, String)
{
  Node node1 = "";
  Node node2 = "some other string";

  Validator v1 = StringType();
  Validator v2 = StringType(isEmptyString);
  Validator v3 = StringType(isNonEmptyString);
  Validator v4 = BooleanType();

  EXPECT_NO_THROW(v1.validate(node1));
  EXPECT_NO_THROW(v2.validate(node1));
  EXPECT_THROW(v3.validate(node1), ValidationException);
  EXPECT_THROW(v4.validate(node1), TypeException);

  EXPECT_NO_THROW(v1.validate(node2));
  EXPECT_THROW(v2.validate(node2), ValidationException);
  EXPECT_NO_THROW(v3.validate(node2));
  EXPECT_THROW(v4.validate(node2), TypeException);
}

TEST(Validator, Sequence)
{
  Node node1 = Sequence({1, 2, 3});
  Node node2 = Sequence({true, false, true});

  Validator v1 = SequenceType();
  Validator v2 = SequenceType(IntegerType());
  Validator v3 = SequenceType(IntegerType(2, 4));
  Validator v4 = SequenceType({IntegerType(0, 1), IntegerType(2, 4)});
  Validator v5 = SequenceType(threeIntegers);
  Validator v6 = BooleanType();

  EXPECT_NO_THROW(v1.validate(node1));
  EXPECT_NO_THROW(v2.validate(node1));
  EXPECT_THROW(v3.validate(node1), ValidationException);
  EXPECT_NO_THROW(v4.validate(node1));
  EXPECT_NO_THROW(v5.validate(node1));
  EXPECT_THROW(v6.validate(node1), TypeException);

  EXPECT_NO_THROW(v1.validate(node2));
  EXPECT_THROW(v2.validate(node2), ValidationException);
  EXPECT_THROW(v3.validate(node2), ValidationException);
  EXPECT_THROW(v4.validate(node2), ValidationException);
  EXPECT_THROW(v5.validate(node2), ValidationException);
  EXPECT_THROW(v6.validate(node2), TypeException);
}

TEST(Validator, Map)
{
  Node node1 = Map({ { "a", Node() } });
  Node node2 = Map({ { "a", Node() },
                     { "b", false } });
  Node node3 = Map({ { "a", true } });
  Node node4 = Map({ { "a", true },
                     { "b", false} });
  Node node5 = Map({ { "b", false } });
  Node node6 = Map();

  Validator v1 = MapType();
  Validator v2 = MapType(MapGroup({ { "a", NullType(), Required } }));
  Validator v3 = MapType(MapGroup({ { "a", NullType(), Optional } }));
  Validator v4 = MapType(MapGroup({ { "a", NullType(), Required } },
                                  NoMoreEntries));
  Validator v5 = MapType(MapGroup({ }, AllowMoreEntries, hasKeyB ));
  Validator v6 = BooleanType();

  EXPECT_NO_THROW(v1.validate(node1));
  EXPECT_NO_THROW(v2.validate(node1));
  EXPECT_NO_THROW(v3.validate(node1));
  EXPECT_NO_THROW(v4.validate(node1));
  EXPECT_THROW(v5.validate(node1), ValidationException); // key B missing
  EXPECT_THROW(v6.validate(node1), TypeException);

  EXPECT_NO_THROW(v1.validate(node2));
  EXPECT_NO_THROW(v2.validate(node2));
  EXPECT_NO_THROW(v3.validate(node2));
  EXPECT_THROW(v4.validate(node2), ValidationException); // extra key b
  EXPECT_NO_THROW(v5.validate(node2));
  EXPECT_THROW(v6.validate(node2), TypeException);

  EXPECT_NO_THROW(v1.validate(node3));
  EXPECT_THROW(v2.validate(node3), TypeException); // wrong type for a
  EXPECT_THROW(v3.validate(node3), TypeException); // wrong type for a
  EXPECT_THROW(v4.validate(node3), TypeException); // wrong type for a
  EXPECT_THROW(v5.validate(node3), ValidationException); // key B missing
  EXPECT_THROW(v6.validate(node3), TypeException);

  EXPECT_NO_THROW(v1.validate(node4));
  EXPECT_THROW(v2.validate(node4), TypeException); // wrong type for a
  EXPECT_THROW(v3.validate(node4), TypeException); // wrong type for a
  EXPECT_THROW(v4.validate(node4), TypeException); // wrong type for a
  EXPECT_NO_THROW(v5.validate(node4));
  EXPECT_THROW(v6.validate(node4), TypeException);

  EXPECT_NO_THROW(v1.validate(node5));
  EXPECT_THROW(v2.validate(node5), ValidationException); // key A missing
  EXPECT_NO_THROW(v3.validate(node5));
  EXPECT_THROW(v4.validate(node5), ValidationException); // key A missing
  EXPECT_NO_THROW(v5.validate(node5));
  EXPECT_THROW(v6.validate(node5), TypeException);

  EXPECT_NO_THROW(v1.validate(node6));
  EXPECT_THROW(v2.validate(node6), ValidationException); // key A missing
  EXPECT_NO_THROW(v3.validate(node6));
  EXPECT_THROW(v4.validate(node6), ValidationException); // key A missing
  EXPECT_THROW(v5.validate(node6), ValidationException); // key B missing
  EXPECT_THROW(v6.validate(node6), TypeException);
}

TEST(Validator, Complex)
{
  Node node(Map({ { "a", Node() },
                  { "b", true },
                  { "c", 25 },
                  { "d", 99.0 },
                  { "e", "some str" },
                  { "f", Sequence({false, 3.141592653589793, 6}) },
                  { "g", Map({ {"aa", 5}, {"bb", "test" } }) },
                  { "h", Sequence({ 3, 4, 5}) }
                }));

  MapType vd1({ MapGroup(
                  {
                    MapEntryType("a", NullType(), Required),
                    MapEntryType("b", BooleanType(), Optional)
                  },
                  AllowMoreEntries),
                MapGroup(
                  {
                    MapEntryType("c", IntegerType(0, 30), Required)
                  },
                  AllowMoreEntries),
                MapGroup(
                  {
                    MapEntryType("h", SequenceType(), Required),
                    MapEntryType("i", MapType(), Optional)
                  },
                  AllowMoreEntries)
              });

  EXPECT_NO_THROW(vd1.validate(node));
}
