option(BUILD_CaPTkInteractiveSegmentationCmdApp "Build command-line app for CaPTkInteractiveSegmentation" ON)

if(BUILD_CaPTkInteractiveSegmentationCmdApp)
  mitkFunctionCreateCommandLineApp(
    NAME CaPTkInteractiveSegmentation
    CPP_FILES "${SRC_FILES}" CaPTkInteractiveSegmentation.cpp
    PACKAGE_DEPENDS ITK OpenCV Qt5|Core+WebEngineWidgets
    DEPENDS PUBLIC MitkCaPTkInteractiveSegmentationModule
  )
endif()
