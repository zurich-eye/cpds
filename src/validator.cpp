/*
 * validator.cpp
 * cpds
 *
 * Copyright (c) 2016 Hannes Friederich.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "cpds/validator.hpp"
#include "cpds/node.hpp"
#include "cpds/exception.hpp"

#include <iostream>

namespace cpds {

// enforce local linkage
namespace {

// accepts all nodes
void vAlways(const Node& /*node*/,
             const Validator& /*validator*/)
{
}

// accepts all maps
bool vAllMaps(const Node& /*node*/)
{
  return true;
}

// integer range validator
void vIntRange(const Node& node,
               const Validator& validator)
{
  IntRange range = validator.intRange();
  Int val = node.intValue();
  if (val < range.first || val > range.second)
  {
    throw IntRangeException(range.first, range.second, val, node);
  }
}

// floating point range validator
void vFloatRange(const Node& node,
                 const Validator& validator)
{
  FloatRange range = validator.floatRange();
  Float val = node.floatValue();
  if (val < range.first || val > range.second)
  {
    throw FloatRangeException(range.first, range.second, val, node);
  }
}

// sequence validator
void vSequence(const Node& node,
               const Validator& validator)
{
  const ValidatorVector& validators = validator.seqValidators();
  if (validators.empty())
  {
    return; // nothing to validate against
  }

  for (const Node& child : node.sequence())
  {
    // any of the validators must succeed
    bool success = false;
    for (const Validator& vld : validators)
    {
      try
      {
        vld.validate(child);
        success = true;
        break;
      }
      catch (...)
      {
      }
    }

    if (!success)
    {
      throw ValidationException("sequence child failed to validate", child);
    }
  }
}

// map validator
void vMap(const Node& node,
          const Validator& validator)
{
  const MapGroupVector& groups = validator.mapGroups();
  if (groups.empty())
  {
    return; // nothing to validate against
  }

  bool enabled = false; // at least one group must be enabled
  for (const MapGroup& group : groups)
  {
    if (group.isEnabled(node))
    {
      group.validate(node);
      enabled = true;
    }
  }

  if (enabled == false)
  {
    throw ValidationException("map does not match any validation group", node);
  }
}

} // unnamed namespace

//
// Validator implementation
//

Validator::Validator(NodeType type, ValidationFcn validation_fcn)
  : type_(type)
  , fcn_(validation_fcn)
{
  aux_data_.int_range_ = nullptr;
}

Validator::Validator(IntRange int_range)
  : type_(NodeType::Integer)
  , fcn_(vIntRange)
{
  aux_data_.int_range_ = new IntRange(int_range);
}

Validator::Validator(FloatRange float_range)
  : type_(NodeType::FloatingPoint)
  , fcn_(vFloatRange)
{
  aux_data_.float_range_ = new FloatRange(float_range);
}

Validator::Validator(ValidatorVector seq_validators)
  : type_(NodeType::Sequence)
  , fcn_(vSequence)
{
  aux_data_.seq_validators_ = new ValidatorVector(std::move(seq_validators));
}

Validator::Validator(MapGroupVector map_groups)
  : type_(NodeType::Map)
  , fcn_(vMap)
{
  aux_data_.map_groups_ = new MapGroupVector(std::move(map_groups));
}

Validator::Validator(const Validator& other)
  : type_(other.type_)
  , fcn_(other.fcn_)
  , aux_data_()
{
  switch (type_)
  {
  case NodeType::Integer:
    if (other.aux_data_.int_range_ != nullptr)
    {
      aux_data_.int_range_ = new IntRange(*other.aux_data_.int_range_);
    }
    break;
  case NodeType::FloatingPoint:
    if (other.aux_data_.float_range_ != nullptr)
    {
      aux_data_.float_range_ = new FloatRange(*other.aux_data_.float_range_);
    }
    break;
  case NodeType::Sequence:
    aux_data_.seq_validators_ =
        new ValidatorVector(*other.aux_data_.seq_validators_);
    break;
  case NodeType::Map:
    aux_data_.map_groups_ =
        new MapGroupVector(*other.aux_data_.map_groups_);
    break;
  default:
    break;
  }
}

Validator::Validator(Validator&& other) noexcept
  : type_(other.type_)
  , fcn_(other.fcn_)
  , aux_data_(other.aux_data_)
{
  other.type_ = NodeType::Null;
  other.aux_data_.int_range_ = nullptr;
}

Validator& Validator::operator=(Validator other) noexcept
{
  swap(other);
  return *this;
}

Validator::~Validator()
{
  switch (type_)
  {
  case NodeType::Integer:
    delete aux_data_.int_range_;
    break;
  case NodeType::FloatingPoint:
    delete aux_data_.float_range_;
    break;
  case NodeType::Sequence:
    delete aux_data_.seq_validators_;
    break;
  case NodeType::Map:
    delete aux_data_.map_groups_;
    break;
  default:
    break;
  }
}

void Validator::validate(const Node& node) const
{
  checkType(node.type());
  fcn_(node, *this);
}

const IntRange& Validator::intRange() const
{
  checkType(NodeType::Integer);
  if (aux_data_.int_range_ == nullptr)
  {
    throw TypeException();
  }
  return *aux_data_.int_range_;
}

const FloatRange& Validator::floatRange() const
{
  checkType(NodeType::FloatingPoint);
  if (aux_data_.float_range_ == nullptr)
  {
    throw TypeException();
  }
  return *aux_data_.float_range_;
}

const ValidatorVector& Validator::seqValidators() const
{
  checkType(NodeType::Sequence);
  return *aux_data_.seq_validators_;
}

const MapGroupVector& Validator::mapGroups() const
{
  checkType(NodeType::Map);
  return *aux_data_.map_groups_;
}

void Validator::swap(Validator& other) noexcept
{
  std::swap(type_, other.type_);
  std::swap(fcn_, other.fcn_);
  std::swap(aux_data_, other.aux_data_);
}

void Validator::checkType(NodeType type) const
{
  if (type_ != type)
  {
    throw TypeException();
  }
}

//
// NullType implementation
//

NullType::NullType()
  : Validator(NodeType::Null, vAlways)
{
}

//
// BooleanType implementation
//

BooleanType::BooleanType()
  : Validator(NodeType::Boolean, vAlways)
{
}

//
// IntegerType implementation
//

IntegerType::IntegerType()
  : Validator(NodeType::Integer, vAlways)
{
}

IntegerType::IntegerType(Int min, Int max)
  : Validator(std::make_pair(min, max))
{
}

IntegerType::IntegerType(ValidationFcn validation_fcn)
  : Validator(NodeType::Integer, validation_fcn)
{
}

//
// FloatingPointType implementation
//

FloatingPointType::FloatingPointType()
  : Validator(NodeType::FloatingPoint, vAlways)
{
}

FloatingPointType::FloatingPointType(Float min, Float max)
  : Validator(std::make_pair(min, max))
{
}

FloatingPointType::FloatingPointType(ValidationFcn validation_fcn)
  : Validator(NodeType::FloatingPoint, validation_fcn)
{
}

//
// StringType implementation
//

StringType::StringType()
  : Validator(NodeType::String, vAlways)
{
}

StringType::StringType(ValidationFcn validation_fcn)
  : Validator(NodeType::String, validation_fcn)
{
}

//
// SequenceType implementation
//

SequenceType::SequenceType()
  : Validator(ValidatorVector())
{
}

SequenceType::SequenceType(Validator child_validator)
  : Validator(ValidatorVector({child_validator}))
{
}

SequenceType::SequenceType(ValidatorVector child_types)
  : Validator(std::move(child_types))
{
}

SequenceType::SequenceType(ValidationFcn validation_fcn)
  : Validator(NodeType::Sequence, validation_fcn)
{
}

//
// MapType implementation
//

MapType::MapType()
  : Validator(MapGroupVector())
{
}

MapType::MapType(MapGroup group)
  : MapType(MapGroupVector{ group })
{
}

MapType::MapType(MapGroupVector groups)
  : Validator(std::move(groups))
{
}

//
// MapEntryType implementation
//

MapEntryType::MapEntryType(String key,
                           Validator validator,
                           Requiredness requiredness)
  : key_(std::move(key))
  , validator_(std::move(validator))
  , requiredness_(requiredness)
{
}

void MapEntryType::validate(const Node& node) const
{
  Map::const_iterator iter = node.find(key_);
  if (iter == node.end())
  {
    if (requiredness_ == Optional)
    {
      return;
    }
    throw ValidationException("required key not present", node);
  }
  validator_.validate(iter->second);
}

//
// MapGroup implementation
//

MapGroup::MapGroup(MapEntryTypeVector entries, Closedness closedness)
  : MapGroup(std::move(entries), closedness, vAllMaps)
{
}

MapGroup::MapGroup(MapEntryTypeVector entries,
                   Closedness closedness,
                   GroupEnableFcn enable_fcn)
  : entries_(std::move(entries))
  , closedness_(closedness)
  , enable_fcn_(enable_fcn)
{
}

bool MapGroup::isEnabled(const Node& node) const
{
  return enable_fcn_(node);
}

void MapGroup::validate(const Node& node) const
{
  for (const MapEntryType& entry : entries_)
  {
    entry.validate(node);
  }

  // ensure there are no other entries
  if (closedness_ == AllowMoreEntries)
  {
    return;
  }

  const Map& map = node.map();
  for (const MapEntry& entry : map)
  {
    const String& key = entry.first;
    bool found = false;
    for (const MapEntryType& type : entries_)
    {
      if (type.key() == key)
      {
        found = true;
        break;
      }
    } // entry type loop

    if (!found)
    {
      throw ValidationException("extra key present in map", node);
    }

  } // map loop
}

} // namespace cpds
