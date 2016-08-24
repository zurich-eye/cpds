/*
 * node.hpp
 * cpds
 *
 * Copyright (c) 2016 Hannes Friederich.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#pragma once

#include <cstdint>
#include <atomic>
#include "cpds/typedefs.hpp"

namespace cpds {

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
  static Node fromScalar(const String& value);
  static Node fromScalar(String&& value);
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
  //@} // Copy & Assignment

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
  bool isNumber() const { return (isInt() || isFloat()); }
  bool isString() const { return type_ == NodeType::String; }
  bool isScalar() const { return type_ == NodeType::Scalar; }
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
   * Throws if the data type is not Boolean and not Scalar.
   * Throws if the scalar cannot be converted to a boolean.
   **/
  bool boolValue() const;

  /**
   * Throws if the data type is not Integer and not Scalar.
   * Throws if the scalar cannot be converted to an Integer.
   **/
  Int intValue() const;

  /**
   * Throws if the data type is not Integer, FloatingPoint, or Scalar.
   * Throws if the integer cannot be represented by a floating point number.
   * Throws if the scalar cannot be converted to a floating point.
   **/
  Float floatValue() const;

  /**
   * Throws if the data type is not String and not Scalar.
   **/
  const String& stringValue() const;

  /**
   * Allows the extraction of complex data types directly from a parent node.
   * See the associated examples.
   **/
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
   * Children access for Maps. Throws for other types.
   *
   * Throws if no entry exists for key.
   **/
  Node& at(const String& key);
  const Node& at(const String& key) const;

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
   * Returns the ID of this node.
   * An ID is assigned every time a node is created from 'real' data.
   * Copying / moving a node does not modify the ID.
   * The ID can be used to associate auxiliary data (e.g. file line number)
   * with a node.
   **/
  uint32_t id() const { return id_; }

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

  static std::atomic<uint32_t> s_id_;

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

  uint32_t _nextId() const;

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
  uint32_t id_;
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
  , id_(_nextId())
  , storage_()
{
}

inline Node::Node(bool value)
  : type_(NodeType::Boolean)
  , id_(_nextId())
  , storage_()
{
  storage_.bool_ = value;
}

inline Node::Node(int value)
  : type_(NodeType::Integer)
  , id_(_nextId())
  , storage_()
{
  storage_.int_ = value;
}

inline Node::Node(long int value)
  : type_(NodeType::Integer)
  , id_(_nextId())
  , storage_()
{
  storage_.int_ = value;
}

inline Node::Node(long long int value)
  : type_(NodeType::Integer)
  , id_(_nextId())
  , storage_()
{
  storage_.int_ = value;
}

inline Node::Node(unsigned int value)
  : type_(NodeType::Integer)
  , id_(_nextId())
  , storage_()
{
  storage_.int_ = value;
}

inline Node::Node(unsigned long int value)
  : type_(NodeType::Integer)
  , id_(_nextId())
  , storage_()
{
  storage_.int_ = value;
}

inline Node::Node(unsigned long long int value)
  : type_(NodeType::Integer)
  , id_(_nextId())
  , storage_()
{
  storage_.int_ = value;
  checkValue(value);
}

inline Node::Node(Float value)
  : type_(NodeType::FloatingPoint)
  , id_(_nextId())
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
  : Node(custom_converter<T>::transform(value))
{
}

template <typename T>
T Node::as() const
{
  return custom_converter<T>::transform(*this);
}

inline uint32_t Node::_nextId() const
{
  return s_id_.fetch_add(1, std::memory_order_relaxed);
}

inline bool operator!=(const Node& lhs, const Node& rhs) noexcept
{
  return !operator==(lhs, rhs);
}

} // namespace cpds
