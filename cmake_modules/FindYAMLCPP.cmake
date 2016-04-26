#
# FindYAMLCPP.cmake
# CMake module to locate yaml-cpp
#
# Copyright (c) 2016 Hannes Friederich
#
# The following variables are set:
#  YAMLCPP_INCLUDE_DIR - the full path to the yaml-cpp headers
#  YAMLCPP_LIBRARIES   - the full path to the yaml-cpp library
#  YAMLCPP_FOUND       - if false, the yaml-cpp library is not found

find_path(YAMLCPP_INCLUDE_DIR yaml-cpp/yaml.h PATH_SUFFIXES include)
find_library(YAMLCPP_LIBRARY yaml-cpp)

set(YAMLCPP_INCLUDE_DIRS ${YAMLCPP_INCLUDE_DIR})
set(YAMLCPP_LIBRARIES ${YAMLCPP_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(YAMLCPP DEFAULT_MSG
                                  YAMLCPP_LIBRARY
                                  YAMLCPP_INCLUDE_DIR)
