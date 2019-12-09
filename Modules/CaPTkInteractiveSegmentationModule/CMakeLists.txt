mitk_create_module(CaPTkInteractiveSegmentationModule
  #DEPENDS PUBLIC MitkSegmentationUI
  #PACKAGE_DEPENDS PRIVATE GuidelinesSupportLibrary
  PACKAGE_DEPENDS Qt5|Core
  DEPENDS MitkCore MitkMultilabel MitkCaPTkCommon
)

add_subdirectory(cmdapps)
