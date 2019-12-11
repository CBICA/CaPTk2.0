# Function to prepend a string to all list items
FUNCTION( MAKE_LIST_OF_PATHS_RELATIVE_TO _listname _list _dir )
    unset( _tmp )
    foreach( item ${_list} )
        file(RELATIVE_PATH _tmpv ${_dir} ${item})
        list(APPEND _tmp "${_tmpv}")
    endforeach( item ${_list} )
    set( ${_listname} ${_tmp} PARENT_SCOPE )
ENDFUNCTION( MAKE_LIST_OF_PATHS_RELATIVE_TO )

FILE(GLOB_RECURSE YAML_ALL "${CMAKE_CURRENT_LIST_DIR}/src/yaml-cpp/*")
FILE(GLOB_RECURSE YAML_HEADERS "${CMAKE_CURRENT_LIST_DIR}/yaml-cpp/files/*.h")
MAKE_LIST_OF_PATHS_RELATIVE_TO(YAML_ALL "${YAML_ALL}" "${CMAKE_CURRENT_LIST_DIR}/src")
MAKE_LIST_OF_PATHS_RELATIVE_TO(YAML_HEADERS "${YAML_HEADERS}" "${CMAKE_CURRENT_LIST_DIR}")

set(CPP_FILES
  CaPTkInteractiveSegmentation.cpp
  CaPTkInteractiveSegmentationAdapter.cpp
  CaPTkInteractiveSegmentationQtPart.cpp

  depends/AdaptiveGeodesicDistance.cpp
  depends/ConfigParserRF.cpp
  depends/ConvertionsOpenCV.cpp
  depends/ConvertionsYAML.cpp
  depends/GeodesicTrainingSegmentation.cpp
  depends/OperationsSvmGTS.cpp
  depends/RandomForestSuite.cpp
  depends/RFPrepareTrainData.cpp
  depends/RFSuiteManager.cpp
  depends/SvmSuite.cpp
  depends/SvmSuiteDescription.cpp
  depends/SvmSuiteManager.cpp
  depends/SvmSuiteOperations.cpp
  depends/SvmSuiteUtil.cpp
  depends/UtilCvMatToImageGTS.cpp
  depends/UtilGTS.cpp
  depends/UtilImageToCvMatGTS.cpp
  depends/UtilItkGTS.cpp 

  ${YAML_ALL}
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

  ${YAML_HEADERS}
)

set(RESOURCE_FILES
  mll_icon2.svg
  cbica-logo.jpg
)
