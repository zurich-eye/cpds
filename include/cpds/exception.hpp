/*
 * exception.hpp
 * cpds
 *
 * Copyright (c) 2016 Hannes Friederich.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#pragma once

#include <exception>
#include <string>
#include <ostream>
#include "cpds/typedefs.hpp"
#include "cpds/parsemark.hpp"

namespace cpds {

class Node;

class Exception : public std::exception
{
public:
  explicit Exception(String msg);
  Exception(String msg, const Node& node);
  Exception(String msg, StringPtr filename, int line, int pos);
  virtual const char* what() const noexcept override;

  const String& message() const { return msg_; }
  uint32_t nodeId() const { return node_id_; }

  bool hasParseMark() const { return parsemark_.valid(); }
  void setParseMark(const ParseMark& pm) { parsemark_ = pm; }
  const String& filename() const { return parsemark_.filename(); }
  int line() const { return parsemark_.line(); }
  int position() const { return parsemark_.position(); }

private:
  String msg_;
  uint32_t node_id_;
  ParseMark parsemark_;
}; // class Exception

class TypeException : public Exception
{
public:
  TypeException();
  explicit TypeException(String msg);
  explicit TypeException(const Node& node);
}; // class TypeException

class OverflowException : public TypeException
{
public:
  OverflowException();
}; // class OverflowException

class KeyException : public Exception
{
public:
  KeyException();
}; // class KeyException

class ImportException : public Exception
{
public:
  ImportException(StringPtr filename, unsigned line, unsigned pos);
  ImportException(String msg, StringPtr filename, unsigned line, unsigned pos);
}; // class ImportException

class ValidationException : public Exception
{
public:
  ValidationException(String msg, const Node& node);
}; // class ValidationException

class IntRangeException : public ValidationException
{
public:
  IntRangeException(Int min, Int max, Int actual, const Node& node);
}; // class IntRangeException

class FloatRangeException : public ValidationException
{
public:
  FloatRangeException(Float min, Float max, Float actual, const Node& node);
}; // class FloatRangeException

std::ostream& operator<<(std::ostream& strm, const Exception& e);

} // namespace cpds
