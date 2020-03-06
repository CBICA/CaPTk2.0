option(BUILD_CaPTkSurvivalPredictionCmdApp "Build command-line app for CaPTkSurvivalPrediction" ON)

if(BUILD_CaPTkSurvivalPredictionCmdApp)
  mitkFunctionCreateCommandLineApp(
    NAME CaPTkSurvivalPredictionCmdApp
    CPP_FILES CaPTkSurvivalPredictionCmdApp.cpp
    PACKAGE_DEPENDS Qt5|Core+Widgets
    DEPENDS MitkCaPTkSurvival
  )
endif()
