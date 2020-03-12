mitk_create_module(
  INCLUDE_DIRS PUBLIC include third_party/jsoncpp/include
  PACKAGE_DEPENDS ITK Qt5|Core PRIVATE yaml-cpp
  DEPENDS PUBLIC MitkCore MitkMultilabel #MitkCaPTkCommon
)
