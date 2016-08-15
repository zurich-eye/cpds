/*
 * parseinfo.cpp
 * cpds
 *
 * Copyright (c) 2016 Hannes Friederich.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#include "cpds/parsemark.hpp"
#include "cpds/parseinfo.hpp"
#include "cpds/node.hpp"

namespace cpds {

//
// ParseMark implementation
//

const String& ParseMark::filename() const
{
  static const String c_unknown_filename = "<unknown>";
  if (!filename_)
  {
    return c_unknown_filename;
  }
  return *filename_;
}

//
// ParseInfo implementation
//

bool ParseInfo::hasMark(const Node& node) const
{
  return hasMark(node.id());
}

bool ParseInfo::hasMark(uint32_t node_id) const
{
  return (find(node_id) != end());
}

const ParseMark& ParseInfo::getMark(const Node& node) const
{
  return getMark(node.id());
}

const ParseMark& ParseInfo::getMark(uint32_t node_id) const
{
  return at(node_id);
}

} // namespace cpds
