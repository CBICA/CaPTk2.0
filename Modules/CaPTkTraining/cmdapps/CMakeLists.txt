#option(BUILD_CaPTkTrainingCmdApp "Build command-line app for CaPTkTraining" ON)

# if(BUILD_CaPTkTrainingCmdApp)
#   mitkFunctionCreateCommandLineApp(
#     NAME CaPTkTrainingCmdApp
#     CPP_FILES "${SRC_FILES}" CaPTkTrainingCmdApp.cpp
#     PACKAGE_DEPENDS ITK OpenCV Qt5|Core+WebEngineWidgets
#     DEPENDS MitkCaPTkTraining
#   )
# endif()
