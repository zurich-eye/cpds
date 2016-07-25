/*
 * yaml.hpp
 * cpds
 *
 * Copyright (c) 2016 Hannes Friederich.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#pragma once

#include <ostream>
#include "cpds/parseinfo.hpp"

namespace YAML {
  class Emitter;
  class Node;
} // namespace YAML

namespace cpds {

class Node;

/**
 * Exports the data structure into YAML format.
 **/
class YamlExport
{
public:
  void dump(std::ostream& strm, const Node& node);
  String dump(const Node& node);
private:
  void dumpNode(YAML::Emitter& emitter, const Node& node) const;
  void dumpFloat(YAML::Emitter& emitter, const Node& node) const;
  void dumpSequence(YAML::Emitter& emitter, const Node& node) const;
  void dumpMap(YAML::Emitter& emitter, const Node& node) const;
}; // class YamlExport

/**
 * Builds a data model from a YAML file
 **/
class YamlImport
{
public:
  Node load(std::istream& strm);
  Node load(const String& str);
  Node loadFromFile(const String& str);

  /**
   * Returns the parse info structure associated with the last parse action
   **/
  const ParseInfo& parseinfo() const { return parseinfo_; }

  /**
   * YAML treats all scalars as strings.
   * When this flag is true (the default), the parser tries to extract
   * booleans, integers and floating point numbers from the string data and
   * create the corresponding type.
   **/
  bool parseScalars() const { return parse_scalars_; }
  void setParseScalars(bool flag) { parse_scalars_ = flag; }

private:
  Node load(std::istream& strm, StringPtr filename);

  Node transform(const YAML::Node& node);
  Node doTransform(const YAML::Node& node);
  Node transformScalar(const YAML::Node& node);
  Node transformSequence(const YAML::Node& node);
  Node transformMap(const YAML::Node& node);

  std::istream* strm_ = nullptr;
  StringPtr filename_;
  ParseInfo parseinfo_;
  bool parse_scalars_ = true;
}; // class YamlImport

} // namespace cpds
