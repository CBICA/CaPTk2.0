mitk_create_module(#CaPTkInteractiveSegmentationModule
  #DEPENDS PUBLIC MitkSegmentationUI
  #PACKAGE_DEPENDS PRIVATE GuidelinesSupportLibrary
  PACKAGE_DEPENDS ITK Qt5|Core
  DEPENDS PUBLIC MitkCore MitkMultilabel MitkExampleModule MitkYamlCpp
)

add_subdirectory(cmdapps)
