/*
 * node.hpp
 * cpds
 *
 * Copyright (c) 2016 Hannes Friederich.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "cpds/node.hpp"
#include <limits>
#include "cpds/exception.hpp"

namespace cpds {

// enforce local linkage
namespace {

struct MapCompare {
  bool operator()(const MapEntry& a, const MapEntry& b) const
  {
    return a.first < b.first;
  }
  bool operator()(const MapEntry& a, const String& key) const
  {
    return a.first < key;
  }
  bool operator()(const String& key, const MapEntry& b) const
  {
    return key < b.first;
  }
}; // struct MapCompare

} // unnamed namespace

Node::Node(const Node& other)
  : type_(other.type_)
  , storage_(other.storage_)
{
  // need to do a deep copy for the pointer types
  switch (type_)
  {
  case NodeType::String:
    new (&storage_) String(other._string());
    break;
  case NodeType::Sequence:
    new (&storage_) Sequence(other._sequence());
    break;
  case NodeType::Map:
    new (&storage_) Map(other._map());
    break;
  default:
    break;
  }
}

Node::Node(Node&& other) noexcept
  : type_(other.type_)
  , storage_(other.storage_)
{
  other.type_ = NodeType::Null; // avoid double free
}

Node& Node::operator=(const Node& other)
{
  Node copy(other);
  swap(copy);
  return *this;
}

Node& Node::operator=(Node&& other) noexcept
{
  swap(other);
  return *this;
}

Node::Node(const String& value)
  : type_(NodeType::String)
  , storage_()
{
  new (&storage_) String(value);
}

Node::Node(String&& value)
  : type_(NodeType::String)
  , storage_()
{
  new (&storage_) String(std::move(value));
}

Node::Node(const Sequence& value)
  : type_(NodeType::Sequence)
  , storage_()
{
  new (&storage_) Sequence(value);
}

Node::Node(Sequence&& value)
  : type_(NodeType::Sequence)
  , storage_()
{
  new (&storage_) Sequence(std::move(value));
}

Node::Node(const Map& value)
  : type_(NodeType::Map)
  , storage_()
{
  new (&storage_) Map(value);

  // the external input is not necessarily sorted
  std::sort(_map().begin(), _map().end(), MapCompare());
}

Node::Node(Map&& value)
  : type_(NodeType::Map)
  , storage_()
{
  new (&storage_) Map(std::move(value));

  // the external input is not necessarily sorted
  std::sort(_map().begin(), _map().end(), MapCompare());
}

Node::~Node()
{
  switch (type_)
  {
  case NodeType::String:
    _string().~String();
    break;
  case NodeType::Sequence:
    _sequence().~Sequence();
    break;
  case NodeType::Map:
    _map().~Map();
    break;
  default:
    break;
  }
}

std::size_t Node::size() const noexcept
{
  switch (type_)
  {
  case NodeType::Sequence:
    return _sequence().size();
  case NodeType::Map:
    return _map().size();
  default:
    return 0;
  }
}

bool Node::boolValue() const
{
  if (type_ != NodeType::Boolean)
  {
    throw TypeException();
  }
  return _bool();
}

Int Node::intValue() const
{
  if (type_ != NodeType::Integer)
  {
    throw TypeException();
  }
  return _int();
}

Float Node::floatValue() const
{
  if (type_ != NodeType::FloatingPoint)
  {
    throw TypeException();
  }
  return _float();
}

const String& Node::stringValue() const
{
  if (type_ != NodeType::String)
  {
    throw TypeException();
  }
  return _string();
}

bool Node::asBool() const
{
  switch(type_)
  {
  case NodeType::Null:
    return false;
  case NodeType::Boolean:
    return _bool();
  case NodeType::Integer:
    return (_int() != 0) ? true : false;
  case NodeType::FloatingPoint:
    return (_float() != 0.0) ? true : false;
  default:
    throw TypeException();
  }
}

Int Node::asInt() const
{
  switch(type_)
  {
  case NodeType::Null:
    return 0;
  case NodeType::Boolean:
    return 0;
  case NodeType::Integer:
    return _int();
  case NodeType::FloatingPoint:
    return _float();
  default:
    throw TypeException();
  }
}

Float Node::asFloat() const
{
  switch(type_)
  {
  case NodeType::Null:
    return 0.0;
  case NodeType::Boolean:
    return 0.0;
  case NodeType::Integer:
    return _int();
  case NodeType::FloatingPoint:
    return _float();
  default:
    throw TypeException();
  }
}

Node& Node::operator[](std::size_t index)
{
  Sequence& seq = sequence();
  if (index >= seq.size())
  {
    throw KeyException();
  }

  return seq[index];
}

const Node& Node::operator[](std::size_t index) const
{
  const Sequence& seq = sequence();
  if (index >= seq.size())
  {
    throw KeyException();
  }

  return seq[index];
}

Sequence& Node::sequence()
{
  if (type_ != NodeType::Sequence)
  {
    throw TypeException();
  }
  return _sequence();
}

const Sequence& Node::sequence() const
{
  if (type_ != NodeType::Sequence)
  {
    throw TypeException();
  }
  return _sequence();
}

Node& Node::operator[](const String& key)
{
  Map& m = map();

  MapCompare comp;
  auto iter = std::lower_bound(m.begin(), m.end(), key, comp);

  if (iter != m.end() && !comp(key, *iter))
  {
    return iter->second;
  }
  return m.insert(iter, std::make_pair(key, Node()))->second;
}

Node& Node::operator[](String&& key)
{
  Map& m = map();

  MapCompare comp;
  auto iter = std::lower_bound(m.begin(), m.end(), key, comp);

  if (iter != m.end() && !comp(key, *iter))
  {
    return iter->second;
  }
  return m.insert(iter, std::make_pair(std::move(key), Node()))->second;
}

Map::iterator Node::find(const String& key)
{
  Map& m = map();

  MapCompare comp;
  auto iter = std::lower_bound(m.begin(), m.end(), key, comp);
  if (iter != m.end() && !comp(key, *iter))
  {
    return iter;
  }
  return m.end();
}

Map::const_iterator Node::find(const String& key) const
{
  const Map& m = map();
  MapCompare comp;
  auto iter = std::lower_bound(m.begin(), m.end(), key, comp);
  if (iter != m.end() && !comp(key, *iter))
  {
    return iter;
  }

  return m.end();
}

Map::iterator Node::end()
{
  return map().end();
}

Map::const_iterator Node::end() const
{
  return map().end();
}

std::size_t Node::erase(const String& key)
{
  Map::iterator iter = find(key);
  if (iter == end())
  {
    return 0;
  }

  _map().erase(iter);
  return 1;
}

bool Node::operator==(const Node& other) const noexcept
{
  if (type_ != other.type_)
  {
    return false;
  }

  switch (type_)
  {
  case NodeType::Null:
    return true;
  case NodeType::Boolean:
    return (_bool() == other._bool());
  case NodeType::Integer:
    return (_int() == other._int());
  case NodeType::FloatingPoint:
    return (_float() == other._float());
  case NodeType::String:
    return (_string() == other._string());
  case NodeType::Sequence:
    return (_sequence() == other._sequence());
  case NodeType::Map:
    return (_map() == other._map());
  }
}

void Node::swap(Node& other) noexcept
{
  std::swap(type_, other.type_);
  std::swap(storage_, other.storage_);
}

const Map& Node::map() const
{
  if (type_ != NodeType::Map)
  {
    throw TypeException();
  }

  return _map();
}

Map& Node::map()
{
  if (type_ != NodeType::Map)
  {
    throw TypeException();
  }
  return _map();
}

inline bool Node::_bool() const
{
  return *reinterpret_cast<const bool*>(&storage_);
}

inline Int Node::_int() const
{
  return *reinterpret_cast<const Int*>(&storage_);
}

inline Float Node::_float() const
{
  return *reinterpret_cast<const Float*>(&storage_);
}

inline const String& Node::_string() const
{
  return *reinterpret_cast<const String*>(&storage_);
}

inline String& Node::_string()
{
  return *reinterpret_cast<String*>(&storage_);
}

inline const Sequence& Node::_sequence() const
{
  return *reinterpret_cast<const Sequence*>(&storage_);
}

inline Sequence& Node::_sequence()
{
  return *reinterpret_cast<Sequence*>(&storage_);
}

inline const Map& Node::_map() const
{
  return *reinterpret_cast<const Map*>(&storage_);
}

inline Map& Node::_map()
{
  return *reinterpret_cast<Map*>(&storage_);
}

void Node::checkValue(unsigned long long int value)
{
  if (value > std::numeric_limits<long long int>::max())
  {
    throw OverflowException();
  }
}


} // namespace cpds
