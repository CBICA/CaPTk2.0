# # Function to prepend a string to all list items
# FUNCTION( MAKE_LIST_OF_PATHS_RELATIVE_TO _listname _list _dir )
#     unset( _tmp )
#     foreach( item ${_list} )
#         file(RELATIVE_PATH _tmpv ${_dir} ${item})
#         list(APPEND _tmp "${_tmpv}")
#     endforeach( item ${_list} )
#     set( ${_listname} ${_tmp} PARENT_SCOPE )
# ENDFUNCTION( MAKE_LIST_OF_PATHS_RELATIVE_TO )

# FILE(GLOB_RECURSE YAML_ALL "${CMAKE_CURRENT_LIST_DIR}/third_party/yaml-cpp/*")
# FILE(GLOB_RECURSE YAML_HEADERS "${CMAKE_CURRENT_LIST_DIR}/yaml-cpp/files/*.h")
# MAKE_LIST_OF_PATHS_RELATIVE_TO(YAML_ALL "${YAML_ALL}" "${CMAKE_CURRENT_LIST_DIR}/src")
# MAKE_LIST_OF_PATHS_RELATIVE_TO(YAML_HEADERS "${YAML_HEADERS}" "${CMAKE_CURRENT_LIST_DIR}")

### Fetch the jsoncpp source files (and make them be relative to src/)
file(GLOB_RECURSE YAMLCPP_FILES RELATIVE 
  "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/third_party/yaml-cpp/*"
)
### Fetch the jsoncpp source files (and make them be relative to src/)
file(GLOB_RECURSE JSONCPP_SOURCE_FILES RELATIVE 
  "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/third_party/jsoncpp/src/*"
)
### Fetch the jsoncpp header files (and make them be relative to module root)
file(GLOB_RECURSE JSONCPP_HEADER_FILES RELATIVE 
  "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/third_party/jsoncpp/include/*"
)

set(CPP_FILES
  CaPTkInteractiveSegmentation.cpp
  CaPTkInteractiveSegmentationAdapter.cpp
  CaPTkInteractiveSegmentationQtPart.cpp

  AdaptiveGeodesicDistance.cpp
  ConfigParserRF.cpp
  ConvertionsOpenCV.cpp
  ConvertionsYAML.cpp
  GeodesicTrainingSegmentation.cpp
  OperationsSvmGTS.cpp
  RandomForestSuite.cpp
  RFPrepareTrainData.cpp
  RFSuiteManager.cpp
  SvmSuite.cpp
  SvmSuiteDescription.cpp
  SvmSuiteManager.cpp
  SvmSuiteOperations.cpp
  SvmSuiteUtil.cpp
  UtilCvMatToImageGTS.cpp
  UtilGTS.cpp
  UtilImageToCvMatGTS.cpp
  UtilItkGTS.cpp 

  ${YAMLCPP_FILES}
  ${JSONCPP_SOURCE_FILES}
)

set(UI_FILES

)

set(MOC_H_FILES
  include/CaPTkInteractiveSegmentation.h
  include/CaPTkInteractiveSegmentationAdapter.h
  include/CaPTkInteractiveSegmentationQtPart.h

  include/AdaptiveGeodesicDistance.h
  include/ConfigParserRF.h
  include/ConvertionsOpenCV.h
  include/ConvertionsYAML.h
  include/GeodesicTrainingSegmentation.h
  include/OperationsSvmGTS.h
  include/Processing.h
  include/RandomForestSuite.h
  include/RFPrepareTrainData.h
  include/RFSuiteManager.h
  include/SusanDenoising.h
  include/SvmSuite.h
  include/SvmSuiteDescription.h
  include/SvmSuiteManager.h
  include/SvmSuiteOperations.h
  include/SvmSuiteUtil.h
  include/UtilCvMatToImageGTS.h
  include/UtilGTS.h
  include/UtilImageToCvMatGTS.h
  include/UtilItkGTS.h

  ${JSONCPP_HEADER_FILES}
)

set(RESOURCE_FILES
  mll_icon2.svg
  cbica-logo.jpg
)
