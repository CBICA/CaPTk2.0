### Fetch the jsoncpp source files (and make them be relative to src/)
file(GLOB_RECURSE YAMLCPP_FILES RELATIVE 
  "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}/third_party/yaml-cpp/*"
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
)

set(RESOURCE_FILES
  mll_icon2.svg
  cbica-logo.jpg
)
