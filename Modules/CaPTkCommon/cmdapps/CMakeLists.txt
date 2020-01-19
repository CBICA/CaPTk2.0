option(BUILD_CaPTkCohortJsonFromFilesystemCmdApp "Build command-line app for CaPTkCohortJsonFromFilesystem" ON)

### Fetch the jsoncpp source files (and make them be relative to src/)
file(GLOB_RECURSE JSONCPP_SOURCE_FILES RELATIVE 
  "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/../third_party/jsoncpp/src/*"
)

if(BUILD_CaPTkCohortJsonFromFilesystemCmdApp)
  mitkFunctionCreateCommandLineApp(
    NAME CaPTkCohortJsonFromFilesystemCmdApp
    CPP_FILES "${JSONCPP_SOURCE_FILES}" CaPTkCohortJsonFromFilesystemCmdApp.cpp
    PACKAGE_DEPENDS ITK Qt5|Core
    DEPENDS MitkCaPTkCommon
  )
endif()
