/*
 * object.hpp
 * cpds
 *
 * Copyright (c) 2016 Hannes Friederich.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#pragma once

#include <string>
#include <vector>

namespace cpds {

class Node;

typedef long long int Int;
typedef double Float;
typedef std::string String;
typedef std::vector<Node> Sequence;
typedef std::pair<std::string, Node> MapEntry;
typedef std::vector<MapEntry> Map;

enum class NodeType
{
  Null,
  Boolean,
  Integer,
  FloatingPoint,
  String,
  Sequence,
  Map,
}; // enum class NodeType

class Node
{
public:
  /**
   * \name Construction From Values
   *
   * The integer overrides are required to disambiguate literals.
   **/
  //@{
  Node(); // null node
  Node(bool value);
  Node(int value);
  Node(long int value);
  Node(long long int value);
  Node(unsigned int value);
  Node(unsigned long int value);
  Node(unsigned long long int value);
  Node(Float value);
  Node(const char* value); // needed or the bool constructor would be used
  Node(const String& value);
  Node(String&& value);
  Node(const Sequence& value);
  Node(Sequence&& value);
  Node(const Map& value);
  Node(Map&& value);
  template <typename T>
  Node(const T& value);
  //@} // Construction From Values

  /**
   * \name Copy & Assignment
   **/
  //@{
  Node(const Node& other);
  Node(Node&& other) noexcept;
  Node& operator=(const Node& other);
  Node& operator=(Node&& other) noexcept;
  //@} // Copy & Assignments

  ~Node() noexcept;

  /**
   * \name Type Information
   **/
  //@{
  NodeType type() const { return type_; }
  bool isNull() const { return type_ == NodeType::Null; }
  bool isBool() const { return type_ == NodeType::Boolean; }
  bool isInt() const { return type_ == NodeType::Integer; }
  bool isFloat() const { return type_ == NodeType::FloatingPoint; }
  bool isString() const { return type_ == NodeType::String; }
  bool isSequence() const { return type_ == NodeType::Sequence; }
  bool isMap() const { return type_ == NodeType::Map; }
  //@} // Type Information

  /**
   * \name Data Access
   **/
  //@{

  /**
   * Returns the size of the underlying container for Sequence & Map.
   * Returns 0 for Null, Boolean, Integer, Floating Point & String.
   **/
  std::size_t size() const noexcept;
  bool empty() const noexcept { return (size() == 0); }

  /**
   * Throws if the data type is not Boolean
   **/
  bool boolValue() const;

  /**
   * Throws if the data type is not Integer
   **/
  Int intValue() const;

  /**
   * Throws if the data type is not FloatingPoint
   **/
  Float floatValue() const;

  /**
   * Throws if the data type is not String
   **/
  const String& stringValue() const;

  /**
   * Transforms the data to a boolean, possibly losing precision.
   * Throws if the type is String, Sequence or Map
   **/
  bool asBool() const;

  /**
   * Transforms the data to an integer, possibly losing precision.
   * Throws if the type is String, Sequence or Map.
   **/
  Int asInt() const;

  /**
   * Transforms the data to a float, possibly losing precision.
   * Throws if the type is String, Sequence or Map.
   **/
  Float asFloat() const;

  template <typename T>
  T as() const;

  /**
   * Children access for Sequences. Throws for other types.
   *
   * Throws if the index is out of bounds.
   **/
  Node& operator[](std::size_t index);
  const Node& operator[](std::size_t index) const;

  /**
   * Access to the underlying data storage. Throws for non-Sequence types.
   **/
  Sequence& sequence();
  const Sequence& sequence() const;

  /**
   * Children access for Maps. Throws for other types.
   *
   * The behaviour is the same as for std::map, i.e. a new entry is created and
   * returned if the key does not exist yet.
   **/
  Node& operator[](const String& key);
  Node& operator[](String&& key);

  /**
   * Key lookup for Maps. Throws for other types.
   **/
  Map::iterator find(const String& key);
  Map::const_iterator find(const String& key) const;
  Map::iterator end();
  Map::const_iterator end() const;

  /**
   * Data removal for Maps. Throws for other types.
   * Returns the number of elements removed.
   **/
  std::size_t erase(const String& key);

  /**
   * Read-only access to the underlying data storage.
   * Throws for non-Map types.
   * Remember that the data storage is actually a std::vector!
   **/
  const Map& map() const;
  //@} // Data Access

  /**
   * Merges the other node into this node.
   *
   * The merge rules are as follows:
   * - Both nodes must have the same type or an exception will be thrown.
   * - For scalars, this node will take on the value of the other node.
   * - For sequences and maps the merge will be performed recursively for each
   *   child of the other node.
   * - For sequences, remaining children of the other node will be copied into
   *   this node.
   * - For maps, all unique keys of the other node will be copied into
   *   this node.
   *
   * This method provides only the basic exception guarantee.
   **/
  void merge(const Node& other);

  void swap(Node& other) noexcept;

  friend bool operator==(const Node& lhs, const Node& rhs) noexcept;

private:

  union Storage
  {
    bool bool_;
    Int int_;
    Float float_;
    String* str_;
    Sequence* seq_;
    Map* map_;
  }; // union Storage

  Map& map();

  bool _bool() const;
  Int _int() const;
  Float _float() const;
  const String& _string() const;
  String& _string();
  const Sequence& _sequence() const;
  Sequence& _sequence();
  const Map& _map() const;
  Map& _map();

  void checkValue(unsigned long long int value);

  void mergeSequence(const Node& other);
  void mergeMap(const Node& other);

  NodeType type_;
  Storage storage_;

}; // class Node

inline void swap(Node& lhs, Node& rhs) noexcept { lhs.swap(rhs); }

bool operator==(const Node& lhs, const Node& hrs) noexcept;
bool operator!=(const Node& lhs, const Node& rhs) noexcept;

//
// inline implementations
//

inline Node::Node()
  : type_(NodeType::Null)
  , storage_()
{
}

inline Node::Node(bool value)
  : type_(NodeType::Boolean)
  , storage_()
{
  storage_.bool_ = value;
}

inline Node::Node(int value)
  : type_(NodeType::Integer)
  , storage_()
{
  storage_.int_ = value;
}

inline Node::Node(long int value)
  : type_(NodeType::Integer)
  , storage_()
{
  storage_.int_ = value;
}

inline Node::Node(long long int value)
  : type_(NodeType::Integer)
  , storage_()
{
  storage_.int_ = value;
}

inline Node::Node(unsigned int value)
  : type_(NodeType::Integer)
  , storage_()
{
  storage_.int_ = value;
}

inline Node::Node(unsigned long int value)
  : type_(NodeType::Integer)
  , storage_()
{
  storage_.int_ = value;
}

inline Node::Node(unsigned long long int value)
  : type_(NodeType::Integer)
  , storage_()
{
  storage_.int_ = value;
  checkValue(value);
}

inline Node::Node(Float value)
  : type_(NodeType::FloatingPoint)
  , storage_()
{
  storage_.float_ = value;
}

inline Node::Node(const char* value)
  : Node(String(value))
{
}

template <typename T>
struct custom_converter;

template <typename T>
Node::Node(const T& value)
  : Node()
{
  *this = custom_converter<T>::transform(value);
}

template <typename T>
T Node::as() const
{
  return custom_converter<T>::transform(*this);
}

inline bool operator!=(const Node& lhs, const Node& rhs) noexcept
{
  return !operator==(lhs, rhs);
}

} // namespace cpds
