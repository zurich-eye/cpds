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
#include <yaml-cpp/yaml.h>
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

std::string YamlExport::dump(const Node& node)
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
  try
  {
    return transform(YAML::Load(strm));
  }
  catch (YAML::Exception& e)
  {
    throw ImportException(0, 0);
  }
}

Node YamlImport::load(const std::string& str)
{
  std::stringstream sstrm(str);
  return load(sstrm);
}

Node YamlImport::transform(const YAML::Node& node) const
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

Node YamlImport::transformScalar(const YAML::Node& node) const
{
  if (parse_scalars_)
  {
    try
    {
      bool value = node.as<bool>();
      return Node(value);
    }
    catch (YAML::Exception& e) { }

    try
    {
      int64_t value = node.as<int64_t>();
      return Node(value);
    }
    catch (YAML::Exception& e) { }

    // special handling for NaN, Inf
    if (node.Scalar() == ".nan")
    {
      return Node(std::numeric_limits<double>::quiet_NaN());
    }
    else if (node.Scalar() == ".inf")
    {
      return Node(std::numeric_limits<double>::infinity());
    }
    else if (node.Scalar() == "-.inf")
    {
      return Node(-std::numeric_limits<double>::infinity());
    }

    try
    {
      double value = node.as<double>();
      return Node(value);
    }
    catch (YAML::Exception& e) { }

  }
  return Node(node.Scalar());
}

Node YamlImport::transformSequence(const YAML::Node& node) const
{
  Sequence seq;
  seq.reserve(node.size());
  for (auto iter = node.begin(); iter != node.end(); ++iter)
  {
    seq.push_back(transform(*iter));
  }
  return Node(std::move(seq));
}

Node YamlImport::transformMap(const YAML::Node& node) const
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
