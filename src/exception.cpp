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

} // namespace cpds
