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
