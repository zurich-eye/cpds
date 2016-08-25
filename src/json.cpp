/*
 * json.cpp
 * cpds
 *
 * Copyright (c) 2016 Hannes Friederich.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "cpds/json.hpp"
#include <cmath>
#include <cassert>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <codecvt>
#include <limits>
#include "cpds/node.hpp"
#include "cpds/exception.hpp"

namespace cpds {

// enforce local linkage
namespace {

inline void updateInteger(uint64_t& integer, char c)
{
  integer *= 10;
  integer += (c-'0');
}

inline bool isNonzeroDigit(char c)
{
  return (c >= '1' && c <= '9');
}

inline bool isDigit(char c)
{
  return (c >= '0' && c <= '9');
}

inline bool isWhitespace(char c)
{
  return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

} // unnamed namespace

//
// JsonExport implementation
//

void JsonExport::dump(std::ostream& strm, const Node& node)
{
  // JSON always starts with an object (aka Map)
  if (!node.isMap())
  {
    throw TypeException();
  }
  offset_ = 0;
  strm << std::setprecision(precision_);
  dumpNode(strm, node);
}

String JsonExport::dump(const Node& node)
{
  std::stringstream sstrm;
  dump(sstrm, node);
  return sstrm.str();
}

void JsonExport::dumpNode(std::ostream& strm, const Node& node)
{
  switch (node.type())
  {
  case NodeType::Null:
    strm << "null";
    break;
  case NodeType::Boolean:
    dumpBoolean(strm, node.boolValue());
    break;
  case NodeType::Integer:
    dumpInteger(strm, node.intValue());
    break;
  case NodeType::FloatingPoint:
    dumpFloat(strm, node.floatValue());
    break;
  case NodeType::String:
    dumpString(strm, node.stringValue());
    break;
  case NodeType::Scalar:
    dumpScalar(strm, node);
    break;
  case NodeType::Sequence:
    dumpSequence(strm, node);
    break;
  case NodeType::Map:
    dumpMap(strm, node);
    break;
  }
}

inline void JsonExport::dumpBoolean(std::ostream& strm, bool value)
{
  strm << (value ? "true" : "false");
}

inline void JsonExport::dumpInteger(std::ostream& strm, Int value)
{
  strm << value;
}

inline void JsonExport::dumpFloat(std::ostream& strm, Float value)
{
  // need to handle +Inf, -Inf, and NaN.
  if (std::isfinite(value))
  {
    // always print a fractional part.
    // this aids identification of floating point numbers when JSON is parsed.
    // std::modf() does not work correctly for numbers near += infinity.
    double intpart;
    strm << value;
    if (std::modf(value, &intpart) == 0.0 &&
        intpart < std::numeric_limits<uint64_t>::max())
    {
      strm << ".0";
    }
  }
  else if (value == std::numeric_limits<double>::infinity())
  {
    strm << std::numeric_limits<double>::max();
  }
  else if (value == -std::numeric_limits<double>::infinity())
  {
    strm << std::numeric_limits<double>::lowest();
  }
  else // NaN
  {
    strm << "null";
  }
}

inline void JsonExport::dumpString(std::ostream& strm, const String& value)
{
  strm << '"';
  for (std::size_t i = 0; i < value.size(); i++)
  {
    char c = value[i];
    if (c == '"')
    {
      strm << "\\\"";
    }
    else if (c == '\\')
    {
      strm << "\\\\";
    }
    else if (c == '/')
    {
      strm << "\\/";
    }
    else if (c == '\b')
    {
      strm << "\\b";
    }
    else if (c == '\f')
    {
      strm << "\\f";
    }
    else if (c == '\n')
    {
      strm << "\\n";
    }
    else if (c == '\r')
    {
      strm << "\\r";
    }
    else if (c == '\t')
    {
      strm << "\\t";
    }
    else if (c >= 0 && c < 0x20) // control character
    {
      dumpHex(strm, c);
    }
    else
    {
      strm << c;
    }
  }
  strm << '"';
}

inline void JsonExport::dumpScalar(std::ostream& strm, const Node& node)
{
  // try to convert to a native data type before emitting a string
  try
  {
    bool val = node.boolValue();
    dumpBoolean(strm, val);
    return;
  }
  catch(const Exception& e)
  {
  }

  try
  {
    Int val = node.intValue();
    dumpInteger(strm, val);
    return;
  }
  catch(const Exception& e)
  {
  }


  try
  {
    Float val = node.floatValue();
    dumpFloat(strm, val);
    return;
  }
  catch(const Exception& e)
  {
  }

  dumpString(strm, node.stringValue());
}

void JsonExport::dumpSequence(std::ostream& strm, const Node& node)
{
  bool first = true;
  const Sequence& seq = node.sequence();

  strm << '[';
  offset_ += indent_;
  dumpOffset(strm);
  for (const Node& n: seq)
  {
    if (!first)
    {
      strm << ',';
      dumpOffset(strm);
    }
    first = false;
    dumpNode(strm, n);
  }
  offset_ -= indent_;
  dumpOffset(strm);
  strm << ']';
}

void JsonExport::dumpMap(std::ostream& strm, const Node& node)
{
  bool first = true;
  const Map& map = node.map();

  strm << '{';
  offset_ += indent_;
  dumpOffset(strm);
  for (auto iter = map.cbegin(); iter != map.cend(); ++iter)
  {
    if (!first)
    {
      strm << ',';
      dumpOffset(strm);
    }
    first = false;
    dumpString(strm, iter->first);
    strm << ':';
    if (indent_)
    {
      strm << ' ';
    }
    dumpNode(strm, iter->second);
  }
  offset_ -= indent_;
  dumpOffset(strm);

  // the trailing bracket of the top-level object goes onto a new line
  if (indent_ != 0 && offset_ == 0)
  {
    strm << '\n';
  }
  strm << '}';
}

void JsonExport::dumpHex(std::ostream& strm, uint16_t value)
{
  const char codes[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
  };
  strm << "\\u";
  strm << codes[(value >> 12) & 0xf];
  strm << codes[(value >>  8) & 0xf];
  strm << codes[(value >>  4) & 0xf];
  strm << codes[(value >>  0) & 0xf];
}

void JsonExport::dumpOffset(std::ostream& strm)
{
  if (offset_ == 0)
  {
    return;
  }
  strm << '\n';
  for (unsigned i = 0; i < offset_; i++)
  {
    strm << ' ';
  }
}

//
// JsonImport implementation
//

Node JsonImport::load(std::istream& strm)
{
  return load(strm, nullptr);
}

Node JsonImport::load(const String& str)
{
  std::stringstream sstrm(str);
  return load(sstrm);
}

Node JsonImport::loadFromFile(const String& filename)
{
  std::ifstream fstrm(filename.c_str());
  return load(fstrm, std::make_shared<String>(filename));
}

Node JsonImport::load(std::istream& strm, StringPtr filename)
{
  // only reset the line and position if the stream changed
  if (strm_ != &strm)
  {
    strm_ = &strm;
    filename_ = filename;
    line_ = 1;
    pos_ = 1;
    parseinfo_.clear();
  }

  skipWs();
  if (peek() != '{')
  {
    raise("not a JSON object");
  }
  return loadMap();
}

Node JsonImport::loadValue()
{
  char c = peek();
  if (c == '"')
  {
    return loadString();
  }
  else if (c == '[')
  {
    return loadSequence();
  }
  else if (c == '{')
  {
    return loadMap();
  }
  else if (c == 't')
  {
    return loadTrue();
  }
  else if (c == 'f')
  {
    return loadFalse();
  }
  else if (c == 'n')
  {
    return loadNull();
  }
  else if (c == '-' || isDigit(c))
  {
    return loadNumber();
  }
  else
  {
    raise();
    return Node();
  }
}

Node JsonImport::loadNull()
{
  ParseMark mark = currentMark();

  if (read() != 'n' ||
      read() != 'u' ||
      read() != 'l' ||
      read() != 'l')
  {
    raise();
  }
  skipWs();
  Node node;
  registerNode(node, std::move(mark));
  return node;
}

Node JsonImport::loadTrue()
{
  ParseMark mark = currentMark();

  if (read() != 't' ||
      read() != 'r' ||
      read() != 'u' ||
      read() != 'e')
  {
    raise();
  }
  skipWs();

  Node node(true);
  registerNode(node, std::move(mark));
  return node;
}

Node JsonImport::loadFalse()
{
  ParseMark mark = currentMark();

  if (read() != 'f' ||
      read() != 'a' ||
      read() != 'l' ||
      read() != 's' ||
      read() != 'e')
  {
    raise();
  }
  skipWs();

  Node node(false);
  registerNode(node, std::move(mark));
  return node;
}

Node JsonImport::loadNumber()
{
  ParseMark mark = currentMark();

  bool is_negative = false;
  bool has_fraction = false;
  bool has_exponent = false;

  uint64_t integer = 0; // for the integer parsing
  String number_str; // for float parsing (std::stod)

  char c = read();
  number_str += c;
  assert(c == '-' || isDigit(c));

  //
  // (optional) sign parse
  //

  if (c == '-')
  {
    is_negative = true;

    c = read();
    number_str += c;
  }

  //
  // integer part
  //

  if (c == '0')
  {
    c = peek();
  }
  else if (isNonzeroDigit(c))
  {
    // the character is already read
    updateInteger(integer, c);
    while (true)
    {
      c = peek();
      if (!isDigit(c))
      {
        break;
      }
      updateInteger(integer, c);

      // consume the character
      read();
      number_str += c;
    }
  }
  else
  {
    raise();
  }

  //
  // optional fraction delimiter
  //

  if (c == '.')
  {
    has_fraction = true;

    // consume the character
    read();
    number_str += c;
    while (true)
    {
      c = peek();
      if (!isDigit(c))
      {
        break;
      }

      // consume the character
      read();
      number_str += c;
    }
  }

  //
  // optional exponent
  //

  if (c == 'e' || c == 'E')
  {
    has_exponent = true;

    // consume the character
    read();
    number_str += c;

    // +, -, or digit, thus consume the character
    c = read();
    number_str += c;
    if (c == '+' || c == '-')
    {
      c = read();
      number_str += c;
    }

    if (!isDigit(c))
    {
      raise();
    }

    while (true)
    {
      c = peek();
      if (!isDigit(c))
      {
        break;
      }
      // consume the character
      read();
      number_str += c;
    }
  }

  //
  // ensure the stream points to the next token past the number
  //

  skipWs();

  //
  // number assembly
  // try to distinguish between integers and floating points
  //

  if (has_fraction || has_exponent)
  {
    // floating point
    std::size_t idx;
    double value = std::stod(number_str, &idx);
    if (idx != number_str.size())
    {
      raise();
    }

    Node node(value);
    registerNode(node, std::move(mark));
    return node;
  }
  else
  {
    // integer
    //
    if (is_negative)
    {
      // explicit cast to ensure the correct Node constructor is used
      Node node(static_cast<Int>(-1*integer));
      parseinfo_.insert(std::make_pair(node.id(), mark));
      return node;
    }

    Node node(integer);
    registerNode(node, std::move(mark));
    return node;
  }
}

Node JsonImport::loadString()
{
  ParseMark mark = currentMark();
  Node node = parseString();
  registerNode(node, std::move(mark));
  return node;
}

Node JsonImport::loadSequence()
{
  ParseMark mark = currentMark();
  Sequence seq;

  char c = read();
  assert(c == '[');
  skipWs();

  c = peek();
  if (c != ']')
  {
    while (true)
    {
      seq.push_back(loadValue());
      c = peek();
      if (c == ']')
      {
        break;
      }
      else if (c == ',')
      {
        read();
        skipWs();
        continue;
      }
      else
      {
        raise();
      }
    } // content loop
  } // empty sequence switch

  assert(c == ']');
  read();
  skipWs();

  Node node(seq);
  registerNode(node, std::move(mark));
  return node;
}

Node JsonImport::loadMap()
{
  ParseMark mark = currentMark();

  Map map;

  char c = read();
  assert(c == '{');
  skipWs();

  c = peek();
  if (c != '}')
  {
    while (true)
    {
      String key = parseString();
      c = read();
      if (c != ':')
      {
        raise();
      }
      skipWs();
      map.push_back(MapEntry(key, loadValue()));
      c = peek();
      if (c == '}')
      {
        break;
      }
      else if (c == ',')
      {
        read();
        skipWs();
        continue;
      }
      else
      {
        raise();
      }
    } // content loop
  } // empty map switch

  assert(c == '}');
  read();
  skipWs();

  Node node(map);
  registerNode(node, std::move(mark));
  return node;
}

String JsonImport::parseString()
{
  String str;
  bool escape = false;

  char c = read();
  if (c != '"')
  {
    raise();
  }

  while (true)
  {
    c = read();
    if (escape == true)
    {
      if (c == '"')
      {
        str += '"';
      }
      else if (c == '\\')
      {
        str += '\\';
      }
      else if (c == '/')
      {
        str += '/';
      }
      else if (c == 'b')
      {
        str += '\b';
      }
      else if (c == 'f')
      {
        str += '\f';
      }
      else if (c == 'n')
      {
        str += '\n';
      }
      else if (c == 'r')
      {
        str += '\r';
      }
      else if (c == 't')
      {
        str += '\t';
      }
      else if (c == 'u')
      {
        uint16_t cp = parseCharacter();
        if (cp <= 0x7f)
        {
          char c1 = (cp >> 0) & 0x7f;
          str += c1;
        }
        else if (cp <= 0x7ff)
        {
          char c1 = ((cp >> 6) & 0x1f) | 0xc0;
          char c2 = ((cp >> 0) & 0x3f) | 0x80;
          str += c1;
          str += c2;
        }
        else
        {
          char c1 = ((cp >> 12) & 0x0f) | 0xe0;
          char c2 = ((cp >>  6) & 0x3f) | 0x80;
          char c3 = ((cp >>  0) & 0x3f) | 0x80;
          str += c1;
          str += c2;
          str += c3;
        }
      }
      else
      {
        raise();
      }
      escape = false;
    }
    else if (c == '"')
    {
      break;
    }
    else if (c == '\\')
    {
      escape = true;
    }
    else
    {
      str += c;
    }
  }
  assert(c == '"');
  skipWs();
  return str;
}

uint16_t JsonImport::parseCharacter()
{
  uint16_t val = 0;
  for (unsigned i = 0; i < 4; i++)
  {
    char c = read();
    val <<= 4;
    if (c >= '0' && c <= '9')
    {
      val += (c - '0');
    }
    else if (c >= 'a' && c <= 'f')
    {
      val += (c - 'a' + 10);
    }
    else if (c >= 'A' && c <= 'F')
    {
      val += (c - 'A' + 10);
    }
    else
    {
      raise("invalid unicode escape");
    }
  }
  return val;
}

char JsonImport::peek()
{
  int c = strm_->peek();
  if (strm_->eof())
  {
    raise("unexpected document end");
  }
  return c;
}

char JsonImport::read()
{
  char c = strm_->get();
  pos_++;
  if (strm_->eof())
  {
    raise("unexpected document end");
  }
  if (c == '\n')
  {
    line_++;
    pos_ = 1;
  }
  return c;
}

void JsonImport::skipWs()
{
  while (true)
  {
    char c = strm_->peek();
    if (strm_->eof() || !isWhitespace(c))
    {
      return;
    }
    read();
  }
}

inline ParseMark JsonImport::currentMark() const
{
  return ParseMark(filename_, line_, pos_);
}

inline void JsonImport::registerNode(const Node& node, ParseMark&& mark)
{
  parseinfo_.insert(std::make_pair(node.id(), std::move(mark)));
}

inline void JsonImport::raise() const
{
  throw ImportException("JSON syntax error", filename_, line_, pos_);
}

inline void JsonImport::raise(const char *msg) const
{
  throw ImportException(msg, filename_, line_, pos_);
}

} // namespace cpds
