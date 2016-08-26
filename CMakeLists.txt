#
# CMakeLists.txt
# cpds
#
# Copyright (c) 2016 Hannes Friederich.
# Copyright (c) ETH Zurich, Wyss Zurich, Zurich Eye
#

project(cpds)
cmake_minimum_required(VERSION 3.1)

find_package(catkin_simple REQUIRED)
catkin_simple(ALL_DEPS_REQUIRED)

set(CMAKE_CXX_STANDARD 11)

set(HEADERS
  include/cpds/exception.hpp
  include/cpds/typedefs.hpp
  include/cpds/node.hpp
  include/cpds/validator.hpp
  include/cpds/parsemark.hpp
  include/cpds/parseinfo.hpp
  include/cpds/json.hpp
  include/cpds/yaml.hpp
)

set(SOURCES
  src/exception.cpp
  src/node.cpp
  src/validator.cpp
  src/parseinfo.cpp
  src/json.cpp
  src/yaml.cpp
)

cs_add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

cs_install()
cs_export()
