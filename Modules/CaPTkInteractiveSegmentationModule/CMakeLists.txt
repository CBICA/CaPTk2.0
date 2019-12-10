mitk_create_module(
  PACKAGE_DEPENDS ITK Qt5|Core
  DEPENDS PUBLIC MitkCore MitkMultilabel MitkExampleModule MitkYamlCpp
)

add_subdirectory(cmdapps)