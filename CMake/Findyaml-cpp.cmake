#[[ Custom find script for Microsofts Guidelines Support Library (GSL) as
    CMake doesn't provide one yet.

    Creates an imported interface target called YamlCppLibrary that
    can be added to the package dependencies of an MITK module or plugin. 

find_path(YamlCppLibrary_INCLUDE_DIR
  NAMES yaml/yaml-cpp
  PATHS ${MITK_EXTERNAL_PROJECT_PREFIX}
  PATH_SUFFIXES include
)

if(NOT TARGET YamlCppLibrary)
  add_library(YamlCppLibrary INTERFACE IMPORTED GLOBAL)
  target_include_directories(YamlCppLibrary INTERFACE ${YamlCppLibrary_INCLUDE_DIR})
  if(CMAKE_CXX_COMPILER_ID MATCHES Clang OR CMAKE_CXX_COMPILER_ID STREQUAL GNU)
    target_compile_options(YamlCppLibrary INTERFACE -fno-strict-aliasing)
  endif()
endif()

find_package_handle_standard_args(YamlCppLibrary
  FOUND_VAR YamlCppLibrary_FOUND
  REQUIRED_VARS YamlCppLibrary_INCLUDE_DIR
)
# - Config file for the yaml-cpp package
# It defines the following variables
#  YAML_CPP_INCLUDE_DIR - include directory
#  YAML_CPP_LIBRARIES    - libraries to link against

# Compute paths
get_filename_component(YAML_CPP_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
set(YAML_CPP_INCLUDE_DIR "${YAML_CPP_CMAKE_DIR}/../include")

message("yaml_cpp_include_dir")
message(${YAML_CPP_INCLUDE_DIR})

# Our library dependencies (contains definitions for IMPORTED targets)
include("${YAML_CPP_CMAKE_DIR}/yaml-cpp-targets.cmake")

# These are IMPORTED targets created by yaml-cpp-targets.cmake
set(YAML_CPP_LIBRARIES "yaml-cpp")
]]