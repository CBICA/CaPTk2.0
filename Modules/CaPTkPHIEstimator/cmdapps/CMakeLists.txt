option(BUILD_CaPTkPHiEstimatorCmdApp "Build command-line apps for the PHI Estimator module" ON)

if(BUILD_CaPTkPHiEstimatorCmdApp)
  mitkFunctionCreateCommandLineApp(
    NAME CaPTkPHIEstimatorCmdApp
    CPP_FILES "${SRC_FILES}" CaPTkPHiEstimatorCmdApp.cpp
    DEPENDS MitkCaPTkPHIEstimator MitkCore
    PACKAGE_DEPENDS PUBLIC ITK|ITKIOImageBase+ITKIONIFTI
  )
endif()
