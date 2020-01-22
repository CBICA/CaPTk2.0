option(BUILD_ExtPHiEstimatorCmdApps "Build command-line apps for the PHI Estimator module" ON)

if(BUILD_ExtPHIEstCmdApps)
  mitkFunctionCreateCommandLineApp(
    NAME ExtPHIEstimatorCmdApp
    DEPENDS PHIEstimator
  )
endif()
