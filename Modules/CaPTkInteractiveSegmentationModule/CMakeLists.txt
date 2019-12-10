mitk_create_module(
  INCLUDE_DIRS PUBLIC include files/include
  PACKAGE_DEPENDS ITK Qt5|Core+Widgets
  DEPENDS MitkCore MitkMultilabel MitkYamlCpp
)

add_subdirectory(cmdapps)