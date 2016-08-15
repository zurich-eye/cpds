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
  const ParseMark& getMark(const Node& node) const;
}; // class ParseInfo

} // namespace cpds
