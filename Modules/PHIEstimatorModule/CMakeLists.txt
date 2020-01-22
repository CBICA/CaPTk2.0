mitk_create_module(PHIEstimatorModule
  DEPENDS PUBLIC MitkSegmentationUI
  #PACKAGE_DEPENDS PRIVATE GuidelinesSupportLibrary
)

add_subdirectory(cmdapps)
