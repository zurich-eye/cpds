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
  std::string msg_;
}; // class ImportException

} // namespace cpds
