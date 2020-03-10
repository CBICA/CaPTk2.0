option(BUILD_CaPTkCohortJsonOperationsCmdApp "Build command-line app for CaPTkCohortJsonOperations" ON)

if(BUILD_CaPTkCohortJsonOperationsCmdApp)
  mitkFunctionCreateCommandLineApp(
    NAME CaPTkCohortJsonOperationsCmdApp
    CPP_FILES CaPTkCohortJsonOperationsCmdApp.cpp
    PACKAGE_DEPENDS ITK Qt5|Core
    DEPENDS MitkCaPTkCommon
  )
endif()


option(BUILD_CaPTkROIConstructionCmdApp "Build command-line app for lattice CaPTkROIConstruction" OFF)

if(BUILD_CaPTkROIConstructionCmdApp)
  mitkFunctionCreateCommandLineApp(
    NAME CaPTkROIConstructionCmdApp
    CPP_FILES CaPTkROIConstructionCmdApp.cpp
    PACKAGE_DEPENDS ITK Qt5|Core
    DEPENDS MitkCaPTkCommon
  )
endif()