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
#include "cpds/typedefs.hpp"

namespace cpds {

class Exception : public std::exception
{
}; // class Exception

class TypeException : public Exception
{
public:
  virtual const char* what() const noexcept override;
}; // class TypeException

class OverflowException : public TypeException
{
public:
  virtual const char* what() const noexcept override;
}; // class OverflowException

class KeyException : public Exception
{
public:
  virtual const char* what() const noexcept override;
}; // class KeyException

class ImportException : public Exception
{
public:
  ImportException(unsigned line, unsigned pos);
  ImportException(unsigned line, unsigned pos, const char* const msg);
  virtual const char* what() const noexcept override;
private:
  String msg_;
}; // class ImportException

class ValidationException : public Exception
{
public:
  ValidationException(String msg);
  virtual const char* what() const noexcept override;
private:
  String msg_;
}; // class ValidationException

class IntRangeException : public ValidationException
{
public:
  IntRangeException(Int min, Int max, Int actual);
}; // class IntRangeException

class FloatRangeException : public ValidationException
{
public:
  FloatRangeException(Float min, Float max, Float actual);
}; // class FloatRangeException

} // namespace cpds
