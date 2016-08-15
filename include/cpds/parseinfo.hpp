/*
 * parseinfo.hpp
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
#include "cpds/parsemark.hpp"

namespace cpds {

class ParseInfo : public std::map<int, ParseMark>
{
public:
  bool hasMark(const Node& node) const;
  bool hasMark(uint32_t node_id) const;
  const ParseMark& getMark(const Node& node) const;
  const ParseMark& getMark(uint32_t node_id) const;
}; // class ParseInfo

} // namespace cpds
