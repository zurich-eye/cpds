#
# CMakeLists.txt
# cpds
#
# Copyright (c) 2016 Hannes Friederich.
#

cmake_minimum_required(VERSION 3.1)
project("cpds" CXX)

set (CPDS_MAJOR_VERSION 0)
set (CPDS_MINOR_VERSION 1)
set (CPDS_PATCH_VERSION 0)

set (CPDS_VERSION
  ${CPDS_MAJOR_VERSION}.${CPDS_MINOR_VERSION}.${CPDS_PATCH_VERSION})

set(CMAKE_CXX_STANDARD 11)

# location of custom CMake modules
list (APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)

option(WITH_JSON          "Enable JSON support" ON)
option(WITH_YAML          "Enable YAML support (requires yaml-cpp)" OFF)
option(BUILD_SHARED_LIBS  "Build Shared Libraries" OFF)
option(BUILD_TESTS        "Builds the test suite (requires GTest)" OFF)

if(BUILD_TESTS)
  set(WITH_YAML ON)
endif()

if(WITH_YAML)
  find_package(YAMLCPP REQUIRED)
endif()

if(BUILD_TESTS)
  find_package(GTest REQUIRED)
endif()

include_directories(./include)

set(CORE_FILES
  include/cpds/exception.hpp
  include/cpds/node.hpp
  src/exception.cpp
  src/node.cpp
)

set(JSON_FILES
  include/cpds/json.hpp
  src/json.cpp
)

set(YAML_FILES
  include/cpds/yaml.hpp
  src/yaml.cpp
)

set(TEST_FILES
  test/test_node.cpp
  test/test_json.cpp
  test/test_yaml.cpp
)

set_source_files_properties(
  ${CORE_FILES} ${JSON_FILES} ${YAML_FILES}
  PROPERTIES COMPILE_FLAGS
  "-Werror -Wall -Wextra -pedantic -fno-common -Wshadow"
)

add_library(cpds ${CORE_FILES})
set_target_properties(cpds PROPERTIES VERSION ${CPDS_MAJOR_VERSION})
set_target_properties(cpds PROPERTIES SOVERSION ${CPDS_VERSION})

if(WITH_JSON)
  add_library(cpds-json ${JSON_FILES})
  set_target_properties(cpds-json PROPERTIES VERSION ${CPDS_MAJOR_VERSION})
  set_target_properties(cpds-json PROPERTIES SOVERSION ${CPDS_VERSION})
    target_link_libraries(cpds-json cpds)
endif()

if(WITH_YAML)
  add_library(cpds-yaml ${YAML_FILES})
  target_include_directories(cpds-yaml PUBLIC ${YAMLCPP_INCLUDE_DIRS})
  set_target_properties(cpds-yaml PROPERTIES VERSION ${CPDS_MAJOR_VERSION})
  set_target_properties(cpds-yaml PROPERTIES SOVERSION ${CPDS_VERSION})
    target_link_libraries(cpds-yaml cpds ${YAMLCPP_LIBRARIES})
endif()

if(BUILD_TESTS)
  add_executable(cpds-test ${TEST_FILES})
  target_include_directories(cpds-test PUBLIC ${GTEST_INCLUDE_DIRS})
  target_link_libraries(cpds-test cpds cpds-json cpds-yaml)
  target_link_libraries(cpds-test ${GTEST_BOTH_LIBRARIES})
endif()

message(STATUS "-----------------------------------------")
message(STATUS "CMAKE_BUILD_TYPE  = ${CMAKE_BUILD_TYPE}")
message(STATUS "BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}")
message(STATUS "WITH_JSON         = ${WITH_JSON}")
message(STATUS "WITH_YAML         = ${WITH_YAML}")
message(STATUS "BUILD_TESTS       = ${BUILD_TESTS}")
message(STATUS "-----------------------------------------")
