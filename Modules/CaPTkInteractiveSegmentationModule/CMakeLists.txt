mitk_create_module(
  INCLUDE_DIRS PUBLIC include files
  PACKAGE_DEPENDS ITK Qt5|Core+Widgets
  DEPENDS PUBLIC MitkYamlCpp MitkCore MitkMultilabel
)

add_subdirectory(cmdapps)