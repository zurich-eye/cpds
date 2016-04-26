/*
 * json.hpp
 * cpds
 *
 * Copyright (c) 2016 Hannes Friederich.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#pragma once

#include <cstdint>
#include <ostream>
#include <istream>
#include <string>

namespace cpds {

class Node;

/**
 * Exports the data structure into JSON format.
 *
 * Note:
 * JSON does not know about +-Inf, NaN for floating point numbers.
 * The corresponding values are exported as
 * +Inf: std::numeric_limits<double::max()
 * -Inf: std::numeric_limits<double>::lowest()
 * -NaN: null
 **/
class JsonExport
{
public:
  void dump(std::ostream& strm, const Node& node);
  std::string dump(const Node& node);

  unsigned precision() const { return precision_; }
  void setPrecision(unsigned precision) { precision_ = precision; }

  unsigned getIndent() const { return indent_; }
  void setIndent(unsigned indent) { indent_ = indent; }

private:
  void dumpNode(std::ostream &strm, const Node &node);
  void dumpFloat(std::ostream& strm, double value);
  void dumpString(std::ostream& strm, const std::string& value);
  void dumpSequence(std::ostream& strm, const Node& node);
  void dumpMap(std::ostream& strm, const Node& node);

  void dumpHex(std::ostream& strm, uint16_t value);
  void dumpOffset(std::ostream& strm);

  unsigned precision_ = 6;
  unsigned indent_ = 0;
  unsigned offset_ = 0;
}; // class JsonExport

class JsonImport
{
public:
  Node load(std::istream& strm);
  Node load(const std::string& str);

private:
  Node loadValue();
  Node loadNull();
  Node loadTrue();
  Node loadFalse();
  Node loadNumber();
  Node loadString();
  Node loadSequence();
  Node loadMap();

  std::string parseString();
  uint16_t parseCharacter();

  char peek();
  char read(); // reads the next character
  void skipWs(); // skips whitespace3

  void raise() const;
  void raise(const char* msg) const;

  std::istream* strm_ = nullptr;
  unsigned line_ = 0;
  unsigned pos_ = 0;
}; // class JsonImport

} // namespace cpds
