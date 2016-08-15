/*
 * exception.cpp
 * cpds
 *
 * Copyright (c) 2016 Hannes Friederich.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "cpds/exception.hpp"
#include "cpds/node.hpp"
#include "cpds/parseinfo.hpp"

namespace cpds {

// enforce local linkage
namespace {

String buildIntRangeMsg(Int min, Int max, Int actual)
{
  String msg("value out of bounds: range [");
  msg += std::to_string(min);
  msg += ":";
  msg += std::to_string(max);
  msg += "], actual ";
  msg += std::to_string(actual);
  return msg;
}

String buildFloatRangeMsg(Float min, Float max, Float actual)
{
  String msg("value out of bounds: range [");
  msg += std::to_string(min);
  msg += ":";
  msg += std::to_string(max);
  msg += "], actual ";
  msg += std::to_string(actual);
  return msg;
}

} // unnamed namespace

//
// Exception implementation
//

Exception::Exception(String msg)
  : msg_(std::move(msg))
  , node_id_(std::numeric_limits<uint32_t>::max())
  , parsemark_()
{
}

Exception::Exception(String msg, const Node& node)
  : msg_(std::move(msg))
  , node_id_(node.id())
  , parsemark_()
{
}

Exception::Exception(String msg, StringPtr filename, int line, int pos)
  : msg_(std::move(msg))
  , node_id_(std::numeric_limits<uint32_t>::max())
  , parsemark_(std::move(filename), line, pos)
{
}

const char* Exception::what() const noexcept
{
  return msg_.c_str();
}

//
// TypeException implementation
//

TypeException::TypeException()
  : Exception("data type mismatch")
{
}

TypeException::TypeException(String msg)
  : Exception(std::move(msg))
{
}

//
// OverflowException implementation
//

OverflowException::OverflowException()
  : TypeException("narrowing from unsigned to signed generates overflow")
{
}

//
// KeyException implementation
//

KeyException::KeyException()
  : Exception("key not found in sequence or map")
{
}

//
// ImportException implementation
//

ImportException::ImportException(StringPtr filename,
                                 unsigned line,
                                 unsigned pos)
  : Exception("invalid data format", std::move(filename), line, pos)
{
}

ImportException::ImportException(String msg,
                                 StringPtr filename,
                                 unsigned line,
                                 unsigned pos)
  : Exception(std::move(msg),
              std::move(filename),
              line, pos)
{
}

//
// ValidationException implementation
//

ValidationException::ValidationException(String msg)
  : Exception(std::move(msg))
{
}

//
// IntRangeException implementation

IntRangeException::IntRangeException(Int min, Int max, Int actual)
  : ValidationException(buildIntRangeMsg(min, max, actual))
{
}

//
// FloatRangeException implementation
//

FloatRangeException::FloatRangeException(Float min, Float max, Float actual)
  : ValidationException(buildFloatRangeMsg(min, max, actual))
{
}

//
// Misc.
//

std::ostream& operator<<(std::ostream& strm, const Exception& e)
{
  strm << e.message();
  if (e.hasParseMark())
  {
    strm << ", file '" << e.filename() << "'";
    strm << ", line " << e.line();
    strm << ", position " << e.position();
  }
  return strm;
}

} // namespace cpds
