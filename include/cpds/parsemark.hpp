/*
 * parsemark.hpp
 * cpds
 *
 * Copyright (c) 2016 Hannes Friederich.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#pragma once

#include <memory>
#include <string>
#include <map>
#include "cpds/typedefs.hpp"

namespace cpds {

class ParseMark
{
public:
  explicit ParseMark();
  ParseMark(int line, int pos);
  ParseMark(StringPtr filename, int line, int pos);

  bool valid() const { return (line_ >= 0 && pos_ >= 0); }
  const String& filename() const;
  int line() const { return line_; }
  int position() const { return pos_; }

private:
  StringPtr filename_;
  int line_;
  int pos_;
}; // class ParseMark

//
// inline implementations
//

inline ParseMark::ParseMark()
  : ParseMark(-1, -1)
{
}

inline ParseMark::ParseMark(int line, int pos)
  : ParseMark(nullptr, line, pos)
{
}

inline ParseMark::ParseMark(StringPtr filename,
                            int line,
                            int pos)
  : filename_(std::move(filename))
  , line_(line)
  , pos_(pos)
{
}

} // namespace cpds
