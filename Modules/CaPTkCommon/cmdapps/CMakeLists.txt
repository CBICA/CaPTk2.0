option(BUILD_CaPTkCohortJsonOperationsCmdApp "Build command-line app for CaPTkCohortJsonOperations" ON)

if(BUILD_CaPTkCohortJsonOperationsCmdApp)
  mitkFunctionCreateCommandLineApp(
    NAME CaPTkCohortJsonOperationsCmdApp
    CPP_FILES CaPTkCohortJsonOperationsCmdApp.cpp
    PACKAGE_DEPENDS ITK Qt5|Core
    DEPENDS MitkCaPTkCommon
  )
endif()
