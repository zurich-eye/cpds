/*
 * validator.hpp
 * cpds
 *
 * Copyright (c) 2016 Hannes Friederich.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#pragma once

#include "cpds/typedefs.hpp"

namespace cpds {

class Node;
class Validator;
class MapGroup;
class MapEntryType;

typedef std::vector<MapGroup> MapGroupVector;
typedef std::vector<Validator> ValidatorVector;
typedef std::vector<MapEntryType> MapEntryTypeVector;

typedef std::pair<Int, Int> IntRange;
typedef std::pair<Float, Float> FloatRange;

enum Requiredness
{
  Required,
  Optional
}; // enum Requiredness

enum Closedness
{
  AllowMoreEntries,
  NoMoreEntries
}; // enum Closedness

/**
 * The validator function shall throw if the node does not validate.
 **/
typedef void (*ValidationFcn)(const Node& node,
                              const Validator& validator);

/**
 * The group enable function shall return whether the map group is valid
 * for the given node. Exceptions are treated as returning false.
 **/
typedef bool (*GroupEnableFcn)(const Node& node);

/**
 * Class to validate a node (and its children where applicable).
 * The actual validation logic is provided through function pointers; the class
 * only manages the validation data.
 *
 * Subclasses exist for convenience & readability, but all validation data
 * is stored in the base class itself. This simplifies memory management,
 * as only base instances need to be stored.
 **/
class Validator
{
protected:
  Validator(NodeType type, ValidationFcn validation_fcn);
  Validator(IntRange int_range);
  Validator(FloatRange float_range);
  Validator(ValidatorVector seq_validators);
  Validator(MapGroupVector map_groups);
public:
  Validator(const Validator& other);
  Validator(Validator&& other) noexcept;
  Validator& operator=(Validator other) noexcept;
  ~Validator();
  void validate(const Node& node) const;
  NodeType type() const { return type_; }
  ValidationFcn validationFcn() const { return fcn_; }
  const IntRange& intRange() const;
  const FloatRange& floatRange() const;
  const ValidatorVector& seqValidators() const;
  const MapGroupVector& mapGroups() const;
  void swap(Validator& other) noexcept;

private:
  union Storage {
    IntRange* int_range_;
    FloatRange* float_range_;
    ValidatorVector* seq_validators_;
    MapGroupVector* map_groups_;
  }; // union Storage

  void checkType(NodeType type) const;

  NodeType type_;
  ValidationFcn fcn_;
  Storage aux_data_;
}; // class Validator

/**
 * Convenience wrapper for null types.
 **/
class NullType : public Validator
{
public:
  NullType();
  ~NullType() = default;
}; // class NullType

/**
 * Convenience wrapper for boolean types.
 **/
class BooleanType : public Validator
{
public:
  BooleanType();
  ~BooleanType() = default;
}; // class BooleanType

/**
 * Convenience wrapper for integer types.
 **/
class IntegerType : public Validator
{
public:
  IntegerType();
  IntegerType(Int min, Int max); // min and max are inclusive
  explicit IntegerType(ValidationFcn validation_fcn);
  ~IntegerType() = default;
}; // class IntegerType

class FloatingPointType : public Validator
{
public:
  FloatingPointType();
  FloatingPointType(Float min, Float max); // min and max are inclusive
  explicit FloatingPointType(ValidationFcn validation_fcn);
  ~FloatingPointType() = default;
}; // class FloatingPointType

class StringType : public Validator
{
public:
  StringType();
  explicit StringType(ValidationFcn validation_fcn);
  ~StringType() = default;
}; // class StringType

class SequenceType : public Validator
{
public:
  SequenceType();
  explicit SequenceType(Validator child_type);
  explicit SequenceType(ValidatorVector child_types);
  explicit SequenceType(ValidationFcn validation_fcn);
  ~SequenceType() = default;
}; // class SequenceType

class MapEntryType
{
public:

  MapEntryType(String key,
               Validator validator,
               Requiredness requiredness);

  void validate(const Node& node) const;

  const String& key() const { return key_; }

private:
  String key_;
  Validator validator_;
  Requiredness requiredness_;
}; // class MapEntryTpe

class MapGroup
{
public:

  MapGroup(MapEntryTypeVector entries,
           Closedness closedness=AllowMoreEntries);
  MapGroup(MapEntryTypeVector entries,
           Closedness closedness,
           GroupEnableFcn enable_fcn);

  bool isEnabled(const Node& node) const;
  void validate(const Node& node) const;

private:
  MapEntryTypeVector entries_;
  Closedness closedness_;
  GroupEnableFcn enable_fcn_;
}; // class MapGroup

class MapType : public Validator
{
public:
  MapType();
  MapType(MapGroup group);
  MapType(MapGroupVector groups);
  ~MapType() = default;
}; // class MapType

//
// misc
//

inline void swap(Validator& lhs, Validator& rhs) { lhs.swap(rhs); }

} // namespace cpds
