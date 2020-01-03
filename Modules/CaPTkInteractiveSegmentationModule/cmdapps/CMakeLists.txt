option(BUILD_CaPTkInteractiveSegmentationCmdApp "Build command-line app for CaPTkInteractiveSegmentation" ON)

if(BUILD_CaPTkInteractiveSegmentationCmdApp)
  mitkFunctionCreateCommandLineApp(
    NAME CaPTkInteractiveSegmentation
    DEPENDS MitkCaPTkInteractiveSegmentationModule
  )
endif()
