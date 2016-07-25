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
  EXPECT_EQ(0, mk.line());
  EXPECT_EQ(0, mk.position());
  mk = pi.getMark(node["b"]);
  EXPECT_EQ(1, mk.line());
  EXPECT_EQ(3, mk.position());
  mk = pi.getMark(node["c"]);
  EXPECT_EQ(2, mk.line());
  EXPECT_EQ(3, mk.position());
  mk = pi.getMark(node["g"]["aa"]);
  EXPECT_EQ(10, mk.line());
  EXPECT_EQ(6, mk.position());
}

TEST(YAML, StringDataImport)
{
  YamlImport yaml_import;
  yaml_import.setParseScalars(false);

  std::string str;
  str = "a:\nb: true\nc: 25\nd: 99.2\ne: \"str with ä and / } \\\" \\\\ special"
        "\\n \\x01 chars\"\nf:\n  - false\n  - 3.141592653589793\n  - 6\n"
        "g:\n  aa: 5\n  bb: .inf";

  Node ref_node(Map({ { "a", Node() },
                      { "b", "true" },
                      { "c", "25" },
                      { "d", "99.2" },
                      { "e", "str with ä and / } \" \\ special\n \u0001 chars" },
                      { "f", Sequence({"false", "3.141592653589793", "6"}) },
                      { "g", Map({ {"aa", "5"}, {"bb", ".inf" } }) }
                     }));

  Node node = yaml_import.load(str);

  EXPECT_EQ(ref_node, node);
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
  EXPECT_EQ(0, mk.line());
  EXPECT_EQ(0, mk.position());
  mk = pi.getMark(node["a"]);
  EXPECT_EQ("/tmp/cpds.yaml", mk.filename());
  EXPECT_EQ(1, mk.line());
  EXPECT_EQ(0, mk.position());
  mk = pi.getMark(node["b"]);
  EXPECT_EQ("/tmp/cpds.yaml", mk.filename());
  EXPECT_EQ(1, mk.line());
  EXPECT_EQ(3, mk.position());
  mk = pi.getMark(node["c"]);
  EXPECT_EQ("/tmp/cpds.yaml", mk.filename());
  EXPECT_EQ(2, mk.line());
  EXPECT_EQ(3, mk.position());
}
