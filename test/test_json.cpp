#include <fstream>
#include <gtest/gtest.h>
#include "cpds/node.hpp"
#include "cpds/json.hpp"
#include "cpds/exception.hpp"

using namespace cpds;

// enforce local linkage
namespace {

Node buildTestNode()
{
  Node node(Map({ { "a", Node() },
                  { "b", true },
                  { "c", 25 },
                  { "d", 99.0 },
                  { "e", "str with ä and / } \" \\ special\n \u0001 chars" },
                  { "f", Sequence({false, 3.141592653589793, 6}) },
                  { "g", Map({ {"aa", 5}, {"bb", std::numeric_limits<double>::infinity() } }) }
                }));
  return node;
}

} // unnamed namespace

TEST(JSON, DataExport)
{
  JsonExport json_export;
  std::string exp;
  std::string cmp;
  exp = json_export.dump(buildTestNode());
  cmp = "{\"a\":null,\"b\":true,\"c\":25,\"d\":99.0,\"e\":\"str with ä and"
        " \\/ } \\\" \\\\ special\\n \\u0001 chars\",\"f\":[false,3.14159,6],"
        "\"g\":{\"aa\":5,\"bb\":1.79769e+308}}";
  EXPECT_EQ(cmp, exp);

  json_export.setPrecision(9);
  exp = json_export.dump(buildTestNode());
  cmp = "{\"a\":null,\"b\":true,\"c\":25,\"d\":99.0,\"e\":\"str with ä and"
        " \\/ } \\\" \\\\ special\\n \\u0001 chars\",\"f\":[false,3.14159265,"
        "6],\"g\":{\"aa\":5,\"bb\":1.79769313e+308}}";

  json_export.setIndent(2);
  exp = json_export.dump(buildTestNode());
  cmp = "{\n  \"a\": null,\n  \"b\": true,\n  \"c\": 25,\n  \"d\": 99.0,"
        "\n  \"e\": \"str with ä and \\/ } \\\" \\\\ special\\n \\u0001 "
        "chars\",\n  \"f\": [\n    false,\n    3.14159265,\n    6\n  ],"
        "\n  \"g\": {\n    \"aa\": 5,\n    \"bb\": 1.79769313e+308\n  }\n}";
  EXPECT_EQ(cmp, exp);

  // The top-level node must be a map
  Node node = 5;
  EXPECT_THROW(json_export.dump(node), TypeException);
}

TEST(JSON, DataImport)
{
  JsonImport json_import;
  std::string str;
  str = "{\"a\":null,\"b\":true,\"c\":25,\"d\":99.0,\"e\":\"str with ä and"
        " \\/ } \\\" \\\\ special\\n \\u0001 chars\",\"f\":[false,3.14159,-6],"
        "\"g\":{\"aa\":5,\"bb\":1.79769e+308}}";

  Node node = json_import.load(str);

  JsonExport json_export;
  std::string exp = json_export.dump(node);
  EXPECT_EQ(str, exp);

  // ensure the parse marks are properly placed
  const ParseInfo& pi = json_import.parseinfo();
  ParseMark mk = pi.getMark(node);
  EXPECT_EQ(1, mk.line());
  EXPECT_EQ(1, mk.position());
  mk = pi.getMark(node["a"]);
  EXPECT_EQ(1, mk.line());
  EXPECT_EQ(6, mk.position());
  mk = pi.getMark(node["b"]);
  EXPECT_EQ(1, mk.line());
  EXPECT_EQ(15, mk.position());
  mk = pi.getMark(node["c"]);
  EXPECT_EQ(1, mk.line());
  EXPECT_EQ(24, mk.position());
  mk = pi.getMark(node["d"]);
  EXPECT_EQ(1, mk.line());
  EXPECT_EQ(31, mk.position());
  mk = pi.getMark(node["e"]);
  EXPECT_EQ(1, mk.line());
  EXPECT_EQ(40, mk.position());
  mk = pi.getMark(node["f"]);
  EXPECT_EQ(1, mk.line());
  EXPECT_EQ(96, mk.position());
  mk = pi.getMark(node["f"][0]);
  EXPECT_EQ(1, mk.line());
  EXPECT_EQ(97, mk.position());
  mk = pi.getMark(node["f"][1]);
  EXPECT_EQ(1, mk.line());
  EXPECT_EQ(103, mk.position());
  mk = pi.getMark(node["g"]);
  EXPECT_EQ(1, mk.line());
  EXPECT_EQ(119, mk.position());

  str = "{a:true}";

  EXPECT_THROW(json_import.load(str), ImportException);
}

TEST(JSON, FileImport)
{
  std::string str;
  str = "{\n\"a\":null,\n\"b\":true,\n\"c\":25\n}\n";
  {
    std::ofstream strm("/tmp/cpds.json");
    strm << str;
  }

  JsonImport json_import;
  Node node = json_import.loadFromFile("/tmp/cpds.json");

  EXPECT_TRUE(node.isMap());
  EXPECT_EQ(Node(), node["a"]);
  EXPECT_TRUE(node["b"].asBool());
  EXPECT_EQ(25, node["c"].asInt());

  const ParseInfo& pi = json_import.parseinfo();
  ParseMark mk = pi.getMark(node);
  EXPECT_EQ("/tmp/cpds.json", mk.filename());
  EXPECT_EQ(1, mk.line());
  EXPECT_EQ(1, mk.position());
  mk = pi.getMark(node["a"]);
  EXPECT_EQ("/tmp/cpds.json", mk.filename());
  EXPECT_EQ(2, mk.line());
  EXPECT_EQ(5, mk.position());
  mk = pi.getMark(node["b"]);
  EXPECT_EQ("/tmp/cpds.json", mk.filename());
  EXPECT_EQ(3, mk.line());
  EXPECT_EQ(5, mk.position());
  mk = pi.getMark(node["c"]);
  EXPECT_EQ("/tmp/cpds.json", mk.filename());
  EXPECT_EQ(4, mk.line());
  EXPECT_EQ(5, mk.position());
}
