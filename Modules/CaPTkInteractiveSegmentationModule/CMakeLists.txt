mitk_create_module(
  INCLUDE_DIRS PUBLIC include src/yaml-cpp
  PACKAGE_DEPENDS ITK Qt5|Core+Widgets
  DEPENDS PUBLIC MitkCore MitkMultilabel
)

add_subdirectory(cmdapps)