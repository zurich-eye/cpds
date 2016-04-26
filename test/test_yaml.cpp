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
        "g:\n  aa: 5\n  bb: inf";
  EXPECT_EQ(cmp, exp);
}

TEST(YAML, DefaultDataImport)
{
  YamlImport yaml_import;
  std::string str;
  str = "a:\nb: true\nc: 25\nd: 99.2\ne: \"str with ä and / } \\\" \\\\ special"
        "\\n \\x01 chars\"\nf:\n  - false\n  - 3.141592653589793\n  - 6\n"
        "g:\n  aa: 5\n  bb: inf";

  Node ref_node(Map({ { "a", Node() },
                      { "b", true },
                      { "c", 25 },
                      { "d", 99.2 },
                      { "e", "str with ä and / } \" \\ special\n \u0001 chars" },
                      { "f", Sequence({false, 3.141592653589793, 6}) },
                      { "g", Map({ {"aa", 5}, {"bb", std::numeric_limits<double>::infinity() } }) }
                     }));

  Node node = yaml_import.load(str);

  EXPECT_EQ(ref_node, node);
}

TEST(YAML, StringDataImport)
{
  YamlImport yaml_import;
  yaml_import.setParseScalars(false);

  std::string str;
  str = "a:\nb: true\nc: 25\nd: 99.2\ne: \"str with ä and / } \\\" \\\\ special"
        "\\n \\x01 chars\"\nf:\n  - false\n  - 3.141592653589793\n  - 6\n"
        "g:\n  aa: 5\n  bb: inf";

  Node ref_node(Map({ { "a", Node() },
                      { "b", "true" },
                      { "c", "25" },
                      { "d", "99.2" },
                      { "e", "str with ä and / } \" \\ special\n \u0001 chars" },
                      { "f", Sequence({"false", "3.141592653589793", "6"}) },
                      { "g", Map({ {"aa", "5"}, {"bb", "inf" } }) }
                     }));

  Node node = yaml_import.load(str);

  EXPECT_EQ(ref_node, node);
}
