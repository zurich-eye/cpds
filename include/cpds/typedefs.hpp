/*
 * typedefs.hpp
 * cpds
 *
 * Copyright (c) 2016 Hannes Friederich.
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

#pragma once

#include <string>
#include <vector>
#include <memory>

namespace cpds {

class Node;

typedef long long int Int;
typedef double Float;
typedef std::string String;
typedef std::vector<Node> Sequence;
typedef std::pair<std::string, Node> MapEntry;
typedef std::vector<MapEntry> Map;

typedef std::shared_ptr<String> StringPtr;

enum class NodeType
{
  Null,
  Boolean,
  Integer,
  FloatingPoint,
  String,
  Scalar,
  Sequence,
  Map,
}; // enum class NodeType

} // namespace cpds
