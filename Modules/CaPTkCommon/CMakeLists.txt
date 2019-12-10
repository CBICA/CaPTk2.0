mitk_create_module(
  PACKAGE_DEPENDS ITK OpenCV
  DEPENDS MitkCore MitkYamlCpp
)

add_subdirectory(cmdapps)
