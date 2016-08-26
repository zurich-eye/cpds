/*
 * yaml.cpp
 * cpds
 *
 * Copyright (c) 2016 Hannes Friederich.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "cpds/yaml.hpp"
#include <cmath>
#include <cassert>
#include <fstream>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include <yaml-cpp/yaml.h>
#pragma GCC diagnostic pop
#include "cpds/node.hpp"
#include "cpds/exception.hpp"

namespace cpds {

//
// YamlExport implementation
//

void YamlExport::dump(std::ostream& strm, const Node& node)
{
  YAML::Emitter emitter(strm);
  dumpNode(emitter, node);
}

String YamlExport::dump(const Node& node)
{
  std::stringstream sstrm;
  dump(sstrm, node);
  return sstrm.str();
}

void YamlExport::dumpNode(YAML::Emitter& emitter, const Node& node) const
{
  switch (node.type())
  {
  case NodeType::Null:
    // note: currently this will emit '~' rather than empty
    emitter << YAML::_Null();
    break;
  case NodeType::Boolean:
    emitter << node.boolValue();
    break;
  case NodeType::Integer:
    emitter << node.intValue();
    break;
  case NodeType::FloatingPoint:
    dumpFloat(emitter, node);
    break;
  case NodeType::String:
    emitter << node.stringValue();
    break;
  case NodeType::Sequence:
    dumpSequence(emitter, node);
    break;
  case NodeType::Map:
    dumpMap(emitter, node);
    break;
  }
}

void YamlExport::dumpFloat(YAML::Emitter& emitter, const Node& node) const
{
  double value = node.floatValue();

  if (std::isnan(value))
  {
    emitter << ".nan";
  }
  else if (std::isinf(value))
  {
    if (value < 0)
    {
      emitter << "-.inf";
    }
    else
    {
      emitter << ".inf";
    }
  }
  else
  {
    emitter << value;
  }
}

void YamlExport::dumpSequence(YAML::Emitter& emitter, const Node& node) const
{
  emitter << YAML::BeginSeq;
  for (const Node& n : node.sequence())
  {
    dumpNode(emitter, n);
  }
  emitter << YAML::EndSeq;
}

void YamlExport::dumpMap(YAML::Emitter& emitter, const Node& node) const
{
  emitter << YAML::BeginMap;
  for (const MapEntry& e : node.map())
  {
    emitter << YAML::Key;
    emitter << e.first;
    emitter << YAML::Value;
    dumpNode(emitter, e.second);
  }
  emitter << YAML::EndMap;
}

//
// YamlImport implementation
//

Node YamlImport::load(std::istream& strm)
{
  return load(strm, nullptr);
}

Node YamlImport::load(const String& str)
{
  std::stringstream sstrm(str);
  return load(sstrm);
}

Node YamlImport::loadFromFile(const String& filename)
{
  std::ifstream fstrm(filename.c_str());
  return load(fstrm, std::make_shared<String>(filename));
}

Node YamlImport::load(std::istream &strm, StringPtr filename)
{
  try
  {
    // reset the parse info when the stream changes
    if (strm_ != &strm)
    {
      strm_ = &strm;
      parseinfo_.clear();
    }
    filename_ = filename;
    return transform(YAML::Load(strm));
  }
  catch (YAML::Exception& e)
  {
    throw ImportException(nullptr, 0, 0);
  }
}

Node YamlImport::transform(const YAML::Node& node)
{
  Node n = doTransform(node);
  const YAML::Mark mark = node.Mark();
  ParseMark pm(filename_, mark.line+1, mark.column+1);
  parseinfo_.insert(std::make_pair(n.id(), std::move(pm)));
  return n;
}

Node YamlImport::doTransform(const YAML::Node& node)
{
  assert(node.IsDefined());
  switch (node.Type())
  {
  case YAML::NodeType::Scalar:
    return transformScalar(node);
  case YAML::NodeType::Sequence:
    return transformSequence(node);
  case YAML::NodeType::Map:
    return transformMap(node);
  default:
    return Node(); // null
  }
}

Node YamlImport::transformScalar(const YAML::Node& node)
{
  //
  // following the Core Schema (10.3 of YAML 1.2)
  //

  const String& str = node.Scalar();

  //
  // null
  //

  if (str.empty() || str == "null" || str == "Null" ||
      str == "NULL" || str == "~")
  {
    return Node();
  }

  //
  // booleans
  //

  if (str == "true" || str == "True" || str == "TRUE")
  {
    return true;
  }
  else if (str == "false" || str == "False" || str == "FALSE")
  {
    return false;
  }

  //
  // integers
  //

  if (str[0] == '+' || str[0] == '-' ||
      (str[0] >= '0' && str[0] <= '9'))
  {
    try
    {
      std::size_t pos = 0;
      Int intval = std::stoll(str, &pos, 10);
      if (pos == str.size())
      {
        return intval;
      }
    }
    catch (const std::exception& e)
    {
      // ignore
    }
  }

  if (str.size() >= 3 && str[0] == '0' && str[1] == 'o')
  {
    try
    {
      String valstr = str.substr(2);
      std::size_t pos = 0;
      Int intval = std::stoll(valstr, &pos, 8);
      if (pos == valstr.size())
      {
        return intval;
      }
    }
    catch (const std::exception& e)
    {
      // ignore
    }
  }

  if (str.size() >= 3 && str[0] == '0' && str[1] == 'x')
  {
    try
    {
      String valstr = str.substr(2);
      std::size_t pos = 0;
      Int intval = std::stoll(valstr, &pos, 16);
      if (pos == valstr.size())
      {
        return intval;
      }
    }
    catch (const std::exception& e)
    {
      // ignore
    }
  }

  //
  // floating point
  //

  if (str == ".inf" || str == ".Inf" || str == ".INF")
  {
    return std::numeric_limits<Float>::infinity();
  }

  if (str == "-.inf" || str == "-.Inf" || str == "-.INF")
  {
    return -std::numeric_limits<Float>::infinity();
  }

  if (str == ".nan" || str == ".NaN" || str == ".NAN")
  {
    return std::numeric_limits<Float>::quiet_NaN();
  }

  try
  {
    std::size_t pos;
    Float val = std::stod(str, &pos);
    if (pos == str.size())
    {
      return val;
    }
  }
  catch (const std::exception& e)
  {
    // ignore
  }

  //
  // String
  //

  return str;
}

Node YamlImport::transformSequence(const YAML::Node& node)
{
  Sequence seq;
  seq.reserve(node.size());
  for (auto iter = node.begin(); iter != node.end(); ++iter)
  {
    seq.push_back(transform(*iter));
  }
  return Node(std::move(seq));
}

Node YamlImport::transformMap(const YAML::Node& node)
{
  Map map;
  map.reserve(node.size());
  for (auto iter = node.begin(); iter != node.end(); ++iter)
  {
    map.emplace_back(iter->first.Scalar(), transform(iter->second));
  }
  return Node(std::move(map));
}

} // namespace cpds
