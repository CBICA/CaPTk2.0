#option(BUILD_CaPTkTrainingModuleCmdApp "Build command-line app for CaPTkTrainingModule" ON)

# if(BUILD_CaPTkTrainingModuleCmdApp)
#   mitkFunctionCreateCommandLineApp(
#     NAME CaPTkTrainingModule
#     CPP_FILES "${SRC_FILES}" CaPTkTrainingModule.cpp
#     PACKAGE_DEPENDS ITK OpenCV Qt5|Core+WebEngineWidgets
#     DEPENDS MitkCaPTkTrainingModule
#   )
# endif()
