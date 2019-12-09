mitk_create_module(CaPTkCommon
  #DEPENDS PUBLIC MitkSegmentationUI
  #PACKAGE_DEPENDS PRIVATE GuidelinesSupportLibrary
#  PACKAGE_DEPENDS Qt5|Core
  PACKAGE_DEPENDS ITK|ITKIOImageBase OpenCV
  DEPENDS MitkCore
#  DEPENDS MitkCore MitkMultilabel
)

add_subdirectory(cmdapps)
