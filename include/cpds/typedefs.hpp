/*
 * typedefs.hpp
 * cpds
 *
 * Copyright (c) 2016 Hannes Friederich. All rights reserved.
 */

#pragma once

#include <string>
#include <vector>

namespace cpds {

class Node;

typedef long long int Int;
typedef double Float;
typedef std::string String;
typedef std::vector<Node> Sequence;
typedef std::pair<std::string, Node> MapEntry;
typedef std::vector<MapEntry> Map;

} // namespace cpds
