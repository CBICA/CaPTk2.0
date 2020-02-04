#option(BUILD_CaPTkTrainingCmdApp "Build command-line app for CaPTkTraining" ON)

# if(BUILD_CaPTkTrainingCmdApp)
#   mitkFunctionCreateCommandLineApp(
#     NAME CaPTkTraining
#     CPP_FILES "${SRC_FILES}" CaPTkTraining.cpp
#     PACKAGE_DEPENDS ITK OpenCV Qt5|Core+WebEngineWidgets
#     DEPENDS MitkCaPTkTraining
#   )
# endif()
