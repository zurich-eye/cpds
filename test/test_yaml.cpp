#include <cmath>
#include <fstream>
#include <gtest/gtest.h>
#include "cpds/node.hpp"
#include "cpds/yaml.hpp"
#include "cpds/exception.hpp"

using namespace cpds;

// enforce local linkage
namespace {

Node buildExportNode()
{
  Node node(Map({ { "b", true },
                  { "c", 25 },
                  { "d", 99.2 },
                  { "e", "str with ä and / } \" \\ special\n \u0001 chars" },
                  { "f", Sequence({false, 3.141592653589793, 6}) },
                  { "g", Map({ {"aa", 5}, {"bb", std::numeric_limits<double>::infinity() } }) }
                }));
  return node;
}

} // unnamed namespace

TEST(YAML, DataExport)
{
  YamlExport yaml_export;
  std::string exp;
  std::string cmp;
  exp = yaml_export.dump(buildExportNode());
  cmp = "b: true\nc: 25\nd: 99.2\ne: \"str with ä and / } \\\" \\\\ special"
        "\\n \\x01 chars\"\nf:\n  - false\n  - 3.141592653589793\n  - 6\n"
        "g:\n  aa: 5\n  bb: .inf";
  EXPECT_EQ(cmp, exp);
}

TEST(YAML, DefaultDataImport)
{
  YamlImport yaml_import;
  std::string str;
  str = "a:\nb: true\nc: 25\nd: 99.2\ne: \"str with ä and / } \\\" \\\\ special"
        "\\n \\x01 chars\"\nf:\n  - false\n  - 3.141592653589793\n  - 6\n"
        "g:\n  aa: 5\n  bb: -.inf";

  Node ref_node(Map({ { "a", Node() },
                      { "b", true },
                      { "c", 25 },
                      { "d", 99.2 },
                      { "e", "str with ä and / } \" \\ special\n \u0001 chars" },
                      { "f", Sequence({false, 3.141592653589793, 6}) },
                      { "g", Map({ {"aa", 5}, {"bb", -std::numeric_limits<double>::infinity() } }) }
                     }));

  Node node = yaml_import.load(str);

  EXPECT_EQ(ref_node, node);

  // ensure the parse marks are properly placed
  const ParseInfo& pi = yaml_import.parseinfo();
  ParseMark mk = pi.getMark(node);
  EXPECT_EQ(1, mk.line());
  EXPECT_EQ(1, mk.position());
  mk = pi.getMark(node["b"]);
  EXPECT_EQ(2, mk.line());
  EXPECT_EQ(4, mk.position());
  mk = pi.getMark(node["c"]);
  EXPECT_EQ(3, mk.line());
  EXPECT_EQ(4, mk.position());
  mk = pi.getMark(node["g"]["aa"]);
  EXPECT_EQ(11, mk.line());
  EXPECT_EQ(7, mk.position());
}

TEST(YAML, TagDeduction)
{
  YamlImport yaml_import;
  String str;
  Node node;

  str = "null";
  node = yaml_import.load(str);
  EXPECT_TRUE(node.isNull());

  str = "Null";
  node = yaml_import.load(str);
  EXPECT_TRUE(node.isNull());

  str = "NULL";
  node = yaml_import.load(str);
  EXPECT_TRUE(node.isNull());

  str = "~";
  node = yaml_import.load(str);
  EXPECT_TRUE(node.isNull());

  str = "";
  node = yaml_import.load(str);
  EXPECT_TRUE(node.isNull());

  str = "true";
  node = yaml_import.load(str);
  EXPECT_TRUE(node.boolValue());

  str = "True";
  node = yaml_import.load(str);
  EXPECT_TRUE(node.boolValue());

  str = "TRUE";
  node = yaml_import.load(str);
  EXPECT_TRUE(node.boolValue());

  str = "TrUE";
  node = yaml_import.load(str);
  EXPECT_TRUE(node.isString());

  str = "false";
  node = yaml_import.load(str);
  EXPECT_FALSE(node.boolValue());

  str = "False";
  node = yaml_import.load(str);
  EXPECT_FALSE(node.boolValue());

  str = "FALSE";
  node = yaml_import.load(str);
  EXPECT_FALSE(node.boolValue());

  str = "-4567";
  node = yaml_import.load(str);
  EXPECT_EQ(-4567, node.intValue());

  str = "0o46";
  node = yaml_import.load(str);
  EXPECT_EQ(38, node.intValue());

  str = "0xa5";
  node = yaml_import.load(str);
  EXPECT_EQ(165, node.intValue());

  str = ".inf";
  node = yaml_import.load(str);
  EXPECT_EQ(std::numeric_limits<Float>::infinity(), node.floatValue());

  str = ".Inf";
  node = yaml_import.load(str);
  EXPECT_EQ(std::numeric_limits<Float>::infinity(), node.floatValue());

  str = ".INF";
  node = yaml_import.load(str);
  EXPECT_EQ(std::numeric_limits<Float>::infinity(), node.floatValue());

  str = ".InF";
  node = yaml_import.load(str);
  EXPECT_TRUE(node.isString());

  str = "-.inf";
  node = yaml_import.load(str);
  EXPECT_EQ(-std::numeric_limits<Float>::infinity(), node.floatValue());

  str = "-.Inf";
  node = yaml_import.load(str);
  EXPECT_EQ(-std::numeric_limits<Float>::infinity(), node.floatValue());

  str = "-.INF";
  node = yaml_import.load(str);
  EXPECT_EQ(-std::numeric_limits<Float>::infinity(), node.floatValue());

  str = "-.InF";
  node = yaml_import.load(str);
  EXPECT_TRUE(node.isString());

  str = ".nan";
  node = yaml_import.load(str);
  EXPECT_TRUE(std::isnan(node.floatValue()));

  str = ".NaN";
  node = yaml_import.load(str);
  EXPECT_TRUE(std::isnan(node.floatValue()));

  str = ".NAN";
  node = yaml_import.load(str);
  EXPECT_TRUE(std::isnan(node.floatValue()));

  str = ".nAN";
  node = yaml_import.load(str);
  EXPECT_TRUE(node.isString());

  str = "-4567.2";
  node = yaml_import.load(str);
  EXPECT_DOUBLE_EQ(-4567.2, node.floatValue());

  str = "-0.539e9";
  node = yaml_import.load(str);
  EXPECT_DOUBLE_EQ(-5.39e8, node.floatValue());

  str = "test";
  node = yaml_import.load(str);
  EXPECT_EQ("test", node.stringValue());
}

TEST(YAML, FileImport)
{
  std::string str;
  str = "a:\nb: true\nc: 25";
  {
    std::ofstream strm("/tmp/cpds.yaml");
    strm << str;
  }

  YamlImport yaml_import;
  Node node = yaml_import.loadFromFile("/tmp/cpds.yaml");

  Node ref_node(Map({ { "a", Node() },
                      { "b", true },
                      { "c", 25 },
                    }));

  EXPECT_EQ(ref_node, node);

  const ParseInfo& pi = yaml_import.parseinfo();
  ParseMark mk = pi.getMark(node);
  EXPECT_EQ("/tmp/cpds.yaml", mk.filename());
  EXPECT_EQ(1, mk.line());
  EXPECT_EQ(1, mk.position());
  mk = pi.getMark(node["a"]);
  EXPECT_EQ("/tmp/cpds.yaml", mk.filename());
  EXPECT_EQ(2, mk.line());
  EXPECT_EQ(1, mk.position());
  mk = pi.getMark(node["b"]);
  EXPECT_EQ("/tmp/cpds.yaml", mk.filename());
  EXPECT_EQ(2, mk.line());
  EXPECT_EQ(4, mk.position());
  mk = pi.getMark(node["c"]);
  EXPECT_EQ("/tmp/cpds.yaml", mk.filename());
  EXPECT_EQ(3, mk.line());
  EXPECT_EQ(4, mk.position());
}
