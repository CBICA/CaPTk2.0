mitk_create_module(CaPTkInteractiveSegmentationModule
  DEPENDS PUBLIC MitkSegmentationUI
  PACKAGE_DEPENDS PRIVATE GuidelinesSupportLibrary
)

add_subdirectory(cmdapps)
