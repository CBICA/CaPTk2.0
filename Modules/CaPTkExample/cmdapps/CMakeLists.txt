option(BUILD_CaPTkExampleCmdApp "Build command-line app for CaPTkExample" ON)

if(BUILD_CaPTkExampleCmdApp)
  mitkFunctionCreateCommandLineApp(
    NAME CaPTkExampleCmdApp
    CPP_FILES CaPTkExampleCmdApp.cpp
    PACKAGE_DEPENDS ITK Qt5|Core
    DEPENDS MitkCaPTkExample
  )
endif()
