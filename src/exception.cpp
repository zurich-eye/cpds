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

const char* TypeException::what() const noexcept
{
  return "data type mismatch.";
}

const char* OverflowException::what() const noexcept
{
  return "narrowing from unsigned to signed generates overflow";
}

const char* KeyException::what() const noexcept
{
  return "key not found in sequence or map.";
}

ImportException::ImportException(unsigned line,
                                 unsigned pos)
  : ImportException(line, pos, "invalid data format")
{
}

ImportException::ImportException(unsigned line,
                                 unsigned pos,
                                 const char* const msg)
  : msg_()
{
  msg_ += "line ";
  msg_ += std::to_string(line);
  msg_ += ", position ";
  msg_ += std::to_string(pos);
  msg_ += ": ";
  msg_ += msg;
}

const char* ImportException::what() const noexcept
{
  return msg_.c_str();
}

ValidationException::ValidationException(String msg)
  : msg_(std::move(msg))
{
}

const char* ValidationException::what() const noexcept
{
  return msg_.c_str();
}

IntRangeException::IntRangeException(Int min, Int max, Int actual)
  : ValidationException(buildIntRangeMsg(min, max, actual))
{
}

FloatRangeException::FloatRangeException(Float min, Float max, Float actual)
  : ValidationException(buildFloatRangeMsg(min, max, actual))
{
}

} // namespace cpds
