#[[ This file is empty as everything is already set up in
    CMake/FindGuidelinesSupportLibrary.cmake. However,
    MITK relies on the existence of this file to
    determine if the package was found. ]]
	
list(APPEND ALL_LIBRARIES ${YAML_CPP_LIBRARIES})
if(YAML_CPP_INCLUDE_DIR)
  list(APPEND ALL_INCLUDE_DIRECTORIES ${YAML_CPP_INCLUDE_DIR})
endif()